vec3 palette( float t) {
  vec3 a = vec3(0.5, 0.5, 0.5);
  vec3 b = vec3(0.5, 0.5, 0.5);
  vec3 c = vec3(1.0, 1.0, 1.0);
  vec3 d = vec3(0.263, 0.416, 0.557);
  return a = b*cos(6.28318*(c*t + d));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
  float u_XMin = -2.0;
  float u_YMin = -1.2;
  float u_XMax =  1.2;
  float u_YMax =  1.2;

  vec2 uv = fragCoord / iResolution.xy;

  float cx = uv.x*(u_XMax - u_XMin) + u_XMin;
  float cy = uv.y*(u_YMax - u_YMin) + u_YMin;

  int n = 512;

  int i;
  int j = n;

  float zx = cx;
  float zy = cy;

  for (i = 0; i < n; i++) {
    float x = (zx*zx - zy*zy) + cx;
    float y = (zy*zx + zx*zy) + cy;

    if (j == n && (x*x + y*y) > 4.0) {
      j = i;
      x = 0.0;
      y = 0.0;
    }

    zx = x;
    zy = y;
  }

  if (j == n) {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
  else {
    float c = float(j)/float(n - 1);

    fragColor = vec4(palette(32*c) + 0.3, 1.0);
  }
}
