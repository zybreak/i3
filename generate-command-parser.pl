#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab
#
# i3 - an improved tiling window manager
# © 2009 Michael Stapelberg and contributors (see also: LICENSE)
#
# generate-command-parser.pl: script to generate parts of the command parser
# from its specification file parser-specs/commands.spec.
#
# Requires only perl >= 5.10, no modules.

use strict;
use warnings;
use Data::Dumper;
use Getopt::Long;
use v5.10;

my $input = '';
my $prefix = '';
my $result = GetOptions(
    'input=s' => \$input,
    'prefix=s' => \$prefix
);

die qq|Input file "$input" does not exist!| unless -e $input;

# reads in a whole file
sub slurp {
    open my $fh, '<', shift;
    local $/;
    <$fh>;
}

# Stores the different states.
my %states;

my @raw_lines = split("\n", slurp($input));
my @lines;

# XXX: In the future, we might switch to a different way of parsing this. The
# parser is in many ways not good — one obvious one is that it is hand-crafted
# without a good reason, also it preprocesses lines and forgets about line
# numbers. Luckily, this is just an implementation detail and the specification
# for the i3 command parser is in-tree (not user input).
# -- michael, 2012-01-12

# First step of preprocessing:
# Join token definitions which are spread over multiple lines.
for my $line (@raw_lines) {
    next if $line =~ /^\s*#/ || $line =~ /^\s*$/;

    if ($line =~ /^\s+->/) {
        # This is a continued token definition, append this line to the
        # previous one.
        $lines[$#lines] = $lines[$#lines] . $line;
    } else {
        push @lines, $line;
        next;
    }
}

# First step: We build up the data structure containing all states and their
# token rules.

my $current_state;

for my $line (@lines) {
    if (my ($state) = ($line =~ /^state ([A-Z0-9_]+):$/)) {
        #say "got a new state: $state";
        $current_state = $state;
    } else {
        # Must be a token definition:
        # [identifier = ] <tokens> -> <action>
        #say "token definition: $line";

        my ($identifier, $tokens, $action) =
            ($line =~ /
                ^\s*                  # skip leading whitespace
                ([a-z_]+ \s* = \s*|)  # optional identifier
                (.*?) -> \s*          # token
                (.*)                  # optional action
             /x);

        # Cleanup the identifier (if any).
        $identifier =~ s/^\s*(\S+)\s*=\s*$/$1/g;

        # The default action is to stay in the current state.
        $action = $current_state if length($action) == 0;

        #say "identifier = *$identifier*, token = *$tokens*, action = *$action*";
        for my $token (split(',', $tokens)) {
            # Cleanup trailing/leading whitespace.
            $token =~ s/^\s*//g;
            $token =~ s/\s*$//g;
            my $store_token = {
                token => $token,
                identifier => $identifier,
                next_state => $action,
            };
            if (exists $states{$current_state}) {
                push @{$states{$current_state}}, $store_token;
            } else {
                $states{$current_state} = [ $store_token ];
            }
        }
    }
}

# Second step: Generate the enum values for all states.

# It is important to keep the order the same, so we store the keys once.
# We sort descendingly by length to be able to replace occurrences of the state
# name even when one state’s name is included in another one’s (like FOR_WINDOW
# is in FOR_WINDOW_COMMAND).
my @keys = sort { (length($b) <=> length($a)) or ($a cmp $b) } keys %states;

open(my $enumfh, '>', "GENERATED_${prefix}_enums.cppm");

my %statenum;
#say $enumfh '#pragma once';
say $enumfh "export module i3_${prefix}_old:enums;";
say $enumfh 'export enum class cmdp_state : int {';
my $cnt = 0;
for my $state (@keys, '__CALL') {
    say $enumfh ',' if $cnt > 0;
    print $enumfh "    $state = $cnt";
    $statenum{$state} = $cnt;
    $cnt++;
}
say $enumfh "\n};";
close($enumfh);

# Third step: Generate the call function.
open(my $callfh, '>', "GENERATED_${prefix}_call.cppm");
my $resultname = uc(substr($prefix, 0, 1)) . substr($prefix, 1) . 'ResultIR';
#say $callfh '#pragma once';
say $callfh "module;";
say $callfh "struct criteria_state;";
say $callfh "export module i3_${prefix}_old:call;";
say $callfh 'import std;';
say $callfh "import :${prefix}_directives;";
say $callfh "import i3_${prefix}_base;";
say $callfh "import :enums;";
say $callfh "import :parser_stack;";
#say $callfh '#include "criteria_state.h"';
#say $callfh 'struct criteria_state;';
say $callfh "export void GENERATED_call(criteria_state *criteria_state, stack &stack, const int call_identifier, $resultname &result) {";
say $callfh '    switch (call_identifier) {';
my $call_id = 0;
for my $state (@keys) {
    my $tokens = $states{$state};
    for my $token (@$tokens) {
        next unless $token->{next_state} =~ /^call /;
        my ($cmd) = ($token->{next_state} =~ /^call (.*)/);
        my ($next_state) = ($cmd =~ /; ([A-Z_]+)$/);
        $cmd =~ s/; ([A-Z_]+)$//;
        # Go back to the INITIAL state unless told otherwise.
        $next_state ||= 'INITIAL';
        my $fmt = $cmd;
        # Replace the references to identified literals (like $workspace) with
        # calls to get_string(). Also replaces state names (like FOR_WINDOW)
        # with their ID (useful for cfg_criteria_init(FOR_WINDOW) e.g.).
        $cmd =~ s/$_/$statenum{$_}/g for @keys;
        $cmd =~ s/\$([a-z_]+)/get_string(stack, "$1")/g;
        $cmd =~ s/\&([a-z_]+)/get_long(stack, "$1")/g;
        # For debugging/testing, we print the call using printf() and thus need
        # to generate a format string. The format uses %d for <number>s,
        # literal numbers or state IDs and %s for NULL, <string>s and literal
        # strings.

        # remove the function name temporarily, so that the following
        # replacements only apply to the arguments.
        my ($funcname) = ($fmt =~ /^(.+)\(/);
        $fmt =~ s/^$funcname//;

        $fmt =~ s/$_/%d/g for @keys;
        $fmt =~ s/\$([a-z_]+)/%s/g;
        $fmt =~ s/\&([a-z_]+)/%ld/g;
        $fmt =~ s/"([a-z0-9_]+)"/%s/g;
        $fmt =~ s/(?:-?|\b)[0-9]+\b/%d/g;

        $fmt = $funcname . $fmt;

        say $callfh "         case $call_id:";
        say $callfh "             result.next_state = std::to_underlying(cmdp_state::$next_state);";
        my $real_cmd = $cmd;
        if ($real_cmd =~ /\(\)/) {
            $real_cmd =~ s/\(/(criteria_state, result/;
        } else {
            $real_cmd =~ s/\(/(criteria_state, result, /;
        }
        say $callfh "             $real_cmd;";
        say $callfh "             break;";
        $token->{next_state} = "call $call_id";
        $call_id++;
    }
}
say $callfh '        default:';
say $callfh '            throw new std::runtime_error(std::format("BUG in the parser. state = {}", call_identifier));';
say $callfh '    }';
say $callfh '}';
close($callfh);

# Fourth step: Generate the token datastructures.

open(my $tokfh, '>', "GENERATED_${prefix}_tokens.cppm");
#say $tokfh '#pragma once';
say $tokfh "export module i3_${prefix}_old:tokens;";
#say $tokfh '#include <vector>';
#say $tokfh '#include <map>';
say $tokfh 'import std;';
say $tokfh 'import :enums;';
say $tokfh 'import :cmdp_token;';
say $tokfh 'using namespace std::literals;';

say $tokfh 'export std::map<cmdp_state, std::vector<cmdp_token>> tokens{';

for my $state (@keys) {
    my $tokens = $states{$state};
    say $tokfh '{cmdp_state::' . $state . ', {';
    for my $token (@$tokens) {
        my $call_identifier = 0;
        my $token_name = $token->{token};
        if ($token_name =~ /^'/) {
            # To make the C code simpler, we leave out the trailing single
            # quote of the literal. We can do strdup(literal + 1); then :).
            $token_name =~ s/'$//;
        }
        # Escape double quotes:
        $token_name =~ s,",\\",g;
        my $next_state = $token->{next_state};
        if ($next_state =~ /^call /) {
            ($call_identifier) = ($next_state =~ /^call ([0-9]+)$/);
            $next_state = '__CALL';
        }
        my $identifier;
        # Set $identifier to NULL if there is no identifier
        if ($token->{identifier} eq ""){
            $identifier = "std::nullopt"
        }
        else{
            $identifier = qq|"$token->{identifier}"s|;
        }
        say $tokfh qq|    { "$token_name"s, $identifier, cmdp_state::$next_state, $call_identifier },|;
    }
    say $tokfh '}},';
}

say $tokfh '};';

close($tokfh);
