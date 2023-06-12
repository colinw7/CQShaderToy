#ifdef GL_ES
precision mediump float;
#endif

// Plot a line on Y using a value between 0.0-1.0
float plot(vec2 st) {    
  return smoothstep(0.02, 0.0, abs(st.y - st.x));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
  vec2 st = fragCoord/iResolution;

  float y = st.x;

  vec3 color = vec3(y);

  // Plot a line
  float pct = plot(st);
  color = (1.0-pct)*color+pct*vec3(0.0,1.0,0.0);

  fragColor = vec4(color,1.0);
}
