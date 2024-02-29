using System;
using System.Runtime.CompilerServices;

namespace Hazel
{
    public class Entity
    {
        protected Entity() { ID = 0; }

        //内部构造函数它具有internal访问修饰符。这意味着只有在同一程序集中才能访问该构造函数，而在程序集外部是不可见的
        internal Entity(ulong id)
        {
            // C++通过构造函数传入实体的UUID
            ID = id;
        }

        public readonly ulong ID;//这是一个只读的ulong类型的属性ID，表示实体的唯一标识符。

        public Vector3 Translation
        {
            get
            {
                // Translation get访问器 是调用C++的内部函数 获取 实体的位置
                InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
                return result;
            }
            set
            {
                // Translation set访问器 是调用C++的内部函数 设置 实体的位置
                InternalCalls.TransformComponent_SetTranslation(ID, ref value);
            }
        }

        //where T : Component, new(): 这是一个类型约束，指定了类型参数 T 必须满足的条件。
        //Component 表示类型参数 T 必须是 Component 类或其子类。
        //new() 表示类型参数 T 必须有一个无参的构造函数。
        public bool HasComponent<T>() where T : Component, new()
        {
            //这一行创建了一个 Type 对象 componentType，它存储了类型参数 T 的实际类型。这个 Type 对象用于后续的操作。
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            // 如果当前对象具有指定类型的组件，则创建一个新的类型为 T 的对象 component。
            // 通过 new T() 这个表达式创建了一个类型为 T 的实例，并将其赋值给 component。
            // 此外，它设置了新创建的组件的 Entity 属性为当前对象（即调用此方法的对象）。
            T component = new T() { Entity = this };
            return component;
        }
    }
}
