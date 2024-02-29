namespace Hazel
{
    public class Input
    {
        //调用C++内部函数来判断wasd是否被按下
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyDown(keycode);
        }
    }
}