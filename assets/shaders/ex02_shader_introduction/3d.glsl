#iChannel0 "pickle_cat.png"
// #iChannel0::WrapMode "Clamp"
#iChannel1 "picke_cat_heightmap.jpg"

void main(){
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    float depth = texture(iChannel1, uv).r;
    vec2 offset = -iMouse.xy / iResolution.xy + 0.5;
    gl_FragColor = texture(iChannel0, uv + 0.05 * depth * vec2(cos(iTime), sin(iTime)));
    // gl_FragColor.rgb *= gl_FragColor.a;
}