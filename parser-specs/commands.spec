# vim:ts=2:sw=2:expandtab
#
# i3 - an improved dynamic tiling window manager
# © 2009 Michael Stapelberg and contributors (see also: LICENSE)
#
# parser-specs/commands.spec: Specification file for generate-command-parser.pl
# which will generate the appropriate header files for our C parser.
#
# Use :source highlighting.vim in vim to get syntax highlighting
# for this file.

state INITIAL:
  # We have an end token here for all the commands which just call some
  # function without using an explicit 'end' token.
  end ->
  '[' -> call cmd::criteria_init(); CRITERIA
  'move' -> MOVE
  'exec' -> EXEC
  'exit' -> call cmd::exit()
  'restart' -> call cmd::restart()
  'reload' -> call cmd::reload()
  'debuglog' -> DEBUGLOG
  'border' -> BORDER
  'layout' -> LAYOUT
  'append_layout' -> APPEND_LAYOUT
  'workspace' -> WORKSPACE
  'focus' -> FOCUS
  'kill' -> KILL
  'open' -> call cmd::open()
  'fullscreen' -> FULLSCREEN
  'sticky' -> STICKY
  'split' -> SPLIT
  'floating' -> FLOATING
  'resize' -> RESIZE
  'rename' -> RENAME
  'nop' -> NOP
  'scratchpad' -> SCRATCHPAD
  'title_format' -> TITLE_FORMAT
  'title_window_icon' -> TITLE_WINDOW_ICON
  'mode' -> MODE
  'bar' -> BAR
  'nagbar' -> NAGBAR

state CRITERIA:
  ctype = 'class'       -> CRITERION
  ctype = 'instance'    -> CRITERION
  ctype = 'window_role' -> CRITERION
  ctype = 'con_id'      -> CRITERION
  ctype = 'id'          -> CRITERION
  ctype = 'window_type' -> CRITERION
  ctype = 'title'       -> CRITERION
  ctype = 'urgent'      -> CRITERION
  ctype = 'workspace'   -> CRITERION
  ctype = 'machine'     -> CRITERION
  ctype = 'tiling', 'floating', 'all'
      -> call cmd::criteria_add($ctype, NULL); CRITERIA
  ']' -> call cmd::criteria_match_windows(); INITIAL

state CRITERION:
  '=' -> CRITERION_STR

state CRITERION_STR:
  cvalue = word
      -> call cmd::criteria_add($ctype, $cvalue); CRITERIA

# exec [--no-startup-id] <command>
state EXEC:
  nosn = '--no-startup-id'
      ->
  command = string
      -> call cmd::exec($nosn, $command)

# debuglog toggle|on|off
state DEBUGLOG:
  argument = 'toggle', 'on', 'off'
    -> call cmd::debuglog($argument)

# border normal|pixel [<n>]
# border none|1pixel|toggle
state BORDER:
  border_style = 'normal', 'pixel', 'toggle'
    -> BORDER_WIDTH
  border_style = 'none'
    -> call cmd::border($border_style, 0)
  '1pixel'
    -> call cmd::border("pixel", 1)

state BORDER_WIDTH:
  end
    -> call cmd::border($border_style, -1)
  border_width = number
    -> call cmd::border($border_style, &border_width)

# layout default|stacked|stacking|tabbed|splitv|splith
# layout toggle [split|all]
state LAYOUT:
  layout_mode = 'default', 'stacked', 'stacking', 'tabbed', 'splitv', 'splith'
      -> call cmd::layout($layout_mode)
  'toggle'
      -> LAYOUT_TOGGLE

# layout toggle [split|all]
state LAYOUT_TOGGLE:
  end
      -> call cmd::layout_toggle($toggle_mode)
  toggle_mode = string
      -> call cmd::layout_toggle($toggle_mode)

# append_layout <path>
state APPEND_LAYOUT:
  path = string -> call cmd::append_layout($path)

# workspace next|prev|next_on_output|prev_on_output
# workspace back_and_forth
# workspace [--no-auto-back-and-forth] <name>
# workspace [--no-auto-back-and-forth] number <number>
state WORKSPACE:
  no_auto_back_and_forth = '--no-auto-back-and-forth'
      ->
  direction = 'next_on_output', 'prev_on_output', 'next', 'prev'
      -> call cmd::workspace($direction)
  'back_and_forth'
      -> call cmd::workspace_back_and_forth()
  'number'
      -> WORKSPACE_NUMBER
  workspace = string
      -> call cmd::workspace_name($workspace, $no_auto_back_and_forth)

state WORKSPACE_NUMBER:
  workspace = string
      -> call cmd::workspace_number($workspace, $no_auto_back_and_forth)

# focus left|right|up|down
# focus output <output>
# focus tiling|floating|mode_toggle
# focus parent|child
# focus
state FOCUS:
  direction = 'left', 'right', 'up', 'down'
      -> call cmd::focus_direction($direction)
  direction = 'prev', 'next'
      -> FOCUS_AUTO
  'output'
      -> FOCUS_OUTPUT
  window_mode = 'tiling', 'floating', 'mode_toggle'
      -> call cmd::focus_window_mode($window_mode)
  level = 'parent', 'child'
      -> call cmd::focus_level($level)
  end
      -> call cmd::focus()

state FOCUS_AUTO:
  'sibling'
      -> call cmd::focus_sibling($direction)
  end
      -> call cmd::focus_direction($direction)

state FOCUS_OUTPUT:
  output = string
      -> call cmd::focus_output($output)

# kill [window|client]
state KILL:
  kill_mode = 'window', 'client'
      -> call cmd::kill($kill_mode)
  end
      -> call cmd::kill($kill_mode)

# fullscreen enable|toggle [global]
# fullscreen disable
# fullscreen [global]
state FULLSCREEN:
  action = 'disable'
      -> call cmd::fullscreen($action, "output")
  action = 'enable', 'toggle'
      -> FULLSCREEN_MODE
  action = ''
      -> FULLSCREEN_COMPAT

state FULLSCREEN_MODE:
  mode = 'global'
      -> call cmd::fullscreen($action, $mode)
  end
      -> call cmd::fullscreen($action, "output")

state FULLSCREEN_COMPAT:
  mode = 'global'
      -> call cmd::fullscreen("toggle", $mode)
  end
      -> call cmd::fullscreen("toggle", "output")

# sticky enable|disable|toggle
state STICKY:
  action = 'enable', 'disable', 'toggle'
      -> call cmd::sticky($action)

# split v|h|t|vertical|horizontal|toggle
state SPLIT:
  direction = 'horizontal', 'vertical', 'toggle', 'v', 'h', 't'
      -> call cmd::split($direction)

# floating enable|disable|toggle
state FLOATING:
  floating = 'enable', 'disable', 'toggle'
      -> call cmd::floating($floating)

# resize
state RESIZE:
  way = 'grow', 'shrink'
      -> RESIZE_DIRECTION
  set = 'set'
      -> RESIZE_SET

state RESIZE_DIRECTION:
  direction = 'up', 'down', 'left', 'right', 'width', 'height'
      -> RESIZE_PX

state RESIZE_PX:
  resize_px = number
      -> RESIZE_TILING
  end
      -> call cmd::resize($way, $direction, 10, 10)

state RESIZE_TILING:
  'px'
      ->
  'or'
      -> RESIZE_TILING_OR
  end
      -> call cmd::resize($way, $direction, &resize_px, 0)

state RESIZE_TILING_OR:
  resize_ppt = number
      -> RESIZE_TILING_FINAL

state RESIZE_TILING_FINAL:
  'ppt', end
      -> call cmd::resize($way, $direction, &resize_px, &resize_ppt)

state RESIZE_SET:
  'height'
      -> RESIZE_HEIGHT_GET_NUMBER
  'width'
      ->
  width = number
      -> RESIZE_WIDTH

state RESIZE_WIDTH:
  mode_width = 'px', 'ppt'
      ->
  end
      -> call cmd::resize_set(&width, $mode_width, 0, 0)
  'height'
      -> RESIZE_HEIGHT_GET_NUMBER
  height = number
      -> RESIZE_HEIGHT

state RESIZE_HEIGHT_GET_NUMBER:
  height = number
      -> RESIZE_HEIGHT

state RESIZE_HEIGHT:
  mode_height = 'px', 'ppt'
      ->
  end
      -> call cmd::resize_set(&width, $mode_width, &height, $mode_height)

# rename workspace <name> to <name>
# rename workspace to <name>
state RENAME:
  'workspace'
      -> RENAME_WORKSPACE

state RENAME_WORKSPACE:
  'to'
      -> RENAME_WORKSPACE_LIKELY_TO
  old_name = word
      -> RENAME_WORKSPACE_TO

state RENAME_WORKSPACE_LIKELY_TO:
  'to '
      -> RENAME_WORKSPACE_LIKELY_TO_NEW_NAME
  new_name = word
      -> call cmd::rename_workspace(NULL, $new_name)

state RENAME_WORKSPACE_LIKELY_TO_NEW_NAME:
  new_name = string
      -> call cmd::rename_workspace("to", $new_name)
  end
      -> call cmd::rename_workspace(NULL, "to")

state RENAME_WORKSPACE_TO:
  'to'
      -> RENAME_WORKSPACE_TO_NEW_NAME

state RENAME_WORKSPACE_TO_NEW_NAME:
  new_name = string
      -> call cmd::rename_workspace($old_name, $new_name)


# move <direction> [<amount> [px|ppt]]
# move [window|container] [to] workspace [<str>|next|prev|next_on_output|prev_on_output|current]
# move [window|container] [to] output <str>
# move [window|container] [to] scratchpad
# move workspace to [output] <str>
# move scratchpad
# move [window|container] [to] [absolute] position [ [<pos_x> [px|ppt] <pos_y> [px|ppt] ] | center ]
# move [window|container] [to] position mouse|cursor|pointer
state MOVE:
  'window'
      ->
  'container'
      ->
  'to'
      ->
  no_auto_back_and_forth = '--no-auto-back-and-forth'
      ->
  'workspace'
      -> MOVE_WORKSPACE
  'output'
      -> MOVE_TO_OUTPUT
  'scratchpad'
      -> call cmd::move_scratchpad()
  direction = 'left', 'right', 'up', 'down'
      -> MOVE_DIRECTION
  method = 'position'
      -> MOVE_TO_POSITION
  method = 'absolute'
      -> MOVE_TO_ABSOLUTE_POSITION

state MOVE_DIRECTION:
  amount = number
      -> MOVE_DIRECTION_NUMBER
  end
      -> call cmd::move_direction($direction, 10, "px")

state MOVE_DIRECTION_NUMBER:
  mode = 'px', 'ppt'
      -> call cmd::move_direction($direction, &amount, $mode)
  end
      -> call cmd::move_direction($direction, &amount, "px")

state MOVE_WORKSPACE:
  'to '
      -> MOVE_WORKSPACE_TO_OUTPUT
  workspace = 'next_on_output', 'prev_on_output', 'next', 'prev', 'current'
      -> call cmd::move_con_to_workspace($workspace)
  'back_and_forth'
      -> call cmd::move_con_to_workspace_back_and_forth()
  'number'
      -> MOVE_WORKSPACE_NUMBER
  workspace = string
      -> call cmd::move_con_to_workspace_name($workspace, $no_auto_back_and_forth)

state MOVE_WORKSPACE_NUMBER:
  number = string
      -> call cmd::move_con_to_workspace_number($number, $no_auto_back_and_forth)

state MOVE_TO_OUTPUT:
  output = word
      -> call cmd::move_con_to_output($output, 0); MOVE_TO_OUTPUT
  end
      -> call cmd::move_con_to_output(NULL, 0); INITIAL

state MOVE_WORKSPACE_TO_OUTPUT:
  'output'
      -> MOVE_WORKSPACE_TO_OUTPUT_WORD

state MOVE_WORKSPACE_TO_OUTPUT_WORD:
  output = word
      -> call cmd::move_con_to_output($output, 1); MOVE_WORKSPACE_TO_OUTPUT_WORD
  end
      -> call cmd::move_con_to_output(NULL, 1); INITIAL

state MOVE_TO_ABSOLUTE_POSITION:
  'position'
      -> MOVE_TO_POSITION

state MOVE_TO_POSITION:
  'center'
      -> call cmd::move_window_to_center($method)
  'mouse', 'cursor', 'pointer'
      -> call cmd::move_window_to_mouse()
  coord_x = number
      -> MOVE_TO_POSITION_X

state MOVE_TO_POSITION_X:
  mode_x = 'px', 'ppt'
      ->
  coord_y = number
      -> MOVE_TO_POSITION_Y

state MOVE_TO_POSITION_Y:
  mode_y = 'px', 'ppt'
      -> call cmd::move_window_to_position(&coord_x, $mode_x, &coord_y, $mode_y)
  end
      -> call cmd::move_window_to_position(&coord_x, $mode_x, &coord_y, 0)

# mode <string>
state MODE:
  mode = string
      -> call cmd::mode($mode)

state NOP:
  comment = string
      -> call cmd::nop($comment)
  end
      -> call cmd::nop(NULL)

state SCRATCHPAD:
  'show'
      -> call cmd::scratchpad_show()

state TITLE_FORMAT:
  format = string
      -> call cmd::title_format($format)

state TITLE_WINDOW_ICON:
  'padding'
    -> TITLE_WINDOW_ICON_PADDING
  enable = '1', 'yes', 'true', 'on', 'enable', 'active', '0', 'no', 'false', 'off', 'disable', 'inactive'
    -> call cmd::title_window_icon($enable, 0)

state TITLE_WINDOW_ICON_PADDING:
  end
    -> call cmd::title_window_icon($enable, &padding)
  'px'
    -> call cmd::title_window_icon($enable, &padding)
  padding = number
    ->

# bar (hidden_state hide|show|toggle)|(mode dock|hide|invisible|toggle) [<bar_id>]
state BAR:
  'hidden_state'
      -> BAR_HIDDEN_STATE
  'mode'
      -> BAR_MODE

state BAR_HIDDEN_STATE:
  bar_value = 'hide', 'show', 'toggle'
      -> BAR_HIDDEN_STATE_ID

state BAR_HIDDEN_STATE_ID:
  bar_id = word
      ->
  end
      -> call cmd::bar_hidden_state($bar_value, $bar_id)

state BAR_MODE:
  bar_value = 'dock', 'hide', 'invisible', 'toggle'
      -> BAR_MODE_ID

state BAR_MODE_ID:
  bar_id = word
      ->
  end
      -> call cmd::bar_mode($bar_value, $bar_id)

state NAGBAR:
  type = 'error', 'warning'
      -> NAGBAR_MESSAGE

state NAGBAR_MESSAGE:
  message = word
      -> NAGBAR_FONT

state NAGBAR_FONT:
  font = word
      -> NAGBAR_PRIMARY

state NAGBAR_PRIMARY:
  primary = 'true', 'false'
      -> NAGBAR_BUTTON_LABEL

state NAGBAR_BUTTON_LABEL:
  label = word
      -> NAGBAR_BUTTON_ACTION

state NAGBAR_BUTTON_ACTION:
  action = word
      -> NAGBAR_BUTTON_TERMINAL

state NAGBAR_BUTTON_TERMINAL:
  terminal = 'true', 'false'
      -> call cmd::nagbar($type, $message, $font, $primary, $label, $action, $terminal)
