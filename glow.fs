#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform float time;

void main()
{
    vec4 color = fragColor;
    float glow = 0.5 + 0.5 * sin(gl_FragCoord.x * 0.1 + gl_FragCoord.y * 0.1 + time);
    finalColor = color * vec4(glow, glow, glow, 1.0);
}
