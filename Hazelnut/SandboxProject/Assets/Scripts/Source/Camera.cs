using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Hazel;

namespace Sandbox
{
    public class Camera : Entity
    {
        public Entity OtherEntity;

        void OnUpdate(float ts)
        {
            float speed = 1.0f;
            Vector3 velocity = Vector3.Zero;
            // 内部调用函数，事件是否触发//
            if (Input.IsKeyDown(KeyCode.Up))
                velocity.Y = 1.0f;
            else if (Input.IsKeyDown(KeyCode.Down))
                velocity.Y = -1.0f;

            if (Input.IsKeyDown(KeyCode.Left))
                velocity.X = -1.0f;
            else if (Input.IsKeyDown(KeyCode.Right))
                velocity.X = 1.0f;

            velocity *= speed;

            // Translation get访问器 是调用C++的内部函数 获取 实体的位置
            Vector3 translation = Translation;
            translation += velocity * ts;
            // Translation set访问器 是调用C++的内部函数 设置 实体的位置
            Translation = translation;
        }

    }
}