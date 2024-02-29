namespace Hazel
{
    public struct Vector3
    {
        public float X, Y, Z;

        public static Vector3 Zero => new Vector3(0.0f);

        public Vector3(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
        }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        //这段代码定义了一个属性 XY，它是一个自定义的 Vector2 结构
        public Vector2 XY
        {
            //get => new Vector2(X, Y);: 这是属性的 getter 方法。它使用 lambda 表达式的简写形式，返回一个新的 Vector2 对象，其中的 X 和 Y 分别由当前对象的 X 和 Y 属性的值组成。
            //set { X = value.X; Y = value.Y; }: 这是属性的 setter 方法。它接受一个 Vector2 类型的值，并将其分配给当前对象的 X 和 Y 属性。
            get => new Vector2(X, Y);
            set
            {
                X = value.X;
                Y = value.Y;
            }
        }

        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }

        public static Vector3 operator *(Vector3 vector, float scalar)
        {
            return new Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
        }

    }
}