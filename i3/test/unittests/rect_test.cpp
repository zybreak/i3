#include <gtest/gtest.h>
import rect;

TEST(RectTest, Empty){
    
    Rect r{};

    ASSERT_EQ(r.x, 0);
    ASSERT_EQ(r.y, 0);
    ASSERT_EQ(r.width, 0);
    ASSERT_EQ(r.height, 0);
}

TEST(RectTest, New){

  Rect r{0, 1, 10, 11};

  ASSERT_EQ(r.x, 0);
  ASSERT_EQ(r.y, 1);
  ASSERT_EQ(r.width, 10);
  ASSERT_EQ(r.height, 11);
}

TEST(RectTest, Add){
    
    Rect a{0, 1, 10, 11};
    Rect b{0, 2, 20, 22};
    
    a += a;

    ASSERT_EQ(a, b);
}

TEST(RectTest, Subtract){
    
    Rect a{0, 1, 10, 11};
    Rect b{0, 2, 20, 22};
    
    b -= a;
    
    ASSERT_EQ(a, b);
}

TEST(RectTest, Gt){
    
    Rect a{0, 1, 10, 11};
    Rect b{0, 2, 20, 22};

    ASSERT_TRUE(b > a);
}

TEST(RectTest, Lt){
    
    Rect a{0, 1, 10, 11};
    Rect b{0, 2, 20, 22};
    
    ASSERT_TRUE(a < b);
}

TEST(RectTest, Contains){
    
    Rect a{0, 1, 10, 11};

    ASSERT_TRUE(a.rect_contains(5, 5));
}
