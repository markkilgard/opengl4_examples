varying vec2 position;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    position = gl_Vertex.st;
}