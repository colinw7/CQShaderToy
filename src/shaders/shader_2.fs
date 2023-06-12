void mainImage(out vec4 fragColor, in vec2 fragCoord) {
  vec2 uv = fragCoord / iResolution.xy;

  vec2 pos = (uv.xy-0.5);

  vec2 cir = ((pos.xy*pos.xy+sin(uv.x*18.0+iTime)/
             25.0*sin(uv.y*7.0+iTime*1.5)/1.0)+uv.x*sin(iTime)/16.0+uv.y*sin(iTime*1.2)/16.0);

  float circles = (sqrt(abs(cir.x+cir.y*0.5)*25.0)*5.0);

  fragColor = vec4(sin(circles*1.25+2.0), abs(sin(circles*1.0-1.0)-sin(circles)),
                   abs(sin(circles)*1.0), 1.0);
}
