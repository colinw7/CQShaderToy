#define AA 2.

struct MetaBall{
  float r;
  vec2 pos;
  vec3 col;
};

vec4 BallSDF(MetaBall ball, vec2 uv){
  float dst = ball.r / length(uv - ball.pos);
  return vec4(ball.col * dst, dst);
}

vec3 renderMetaBall(vec2 uv){
  MetaBall  mbr, mbg, mbb;
  mbr.pos = 0.7 * sin(iTime*.5 + vec2(4.0, 0.5) + 6.0); mbr.r = 0.6; mbr.col = vec3(1., 0., 0.);
  mbg.pos = 0.7 * sin(iTime*.8 + vec2(1.0, 25) + 2.0); mbg.r = 0.9; mbg.col = vec3(0., 1., 0.);
  mbb.pos = 0.7 * sin(iTime*1.2 + vec2(3.0, 2.5) + 4.0); mbb.r = 0.75; mbb.col = vec3(0., 0., 1.);
  
  vec4 ballr = BallSDF(mbr, uv);
  vec4 ballg = BallSDF(mbg, uv);
  vec4 ballb = BallSDF(mbb, uv);
    
  float total = ballr.a + ballg.a + ballb.a;
  float threshold = total > 4.5 ? 1. : 0.;
  vec3 color = (ballr.rgb + ballg.rgb + ballb.rgb) / total;
  color *= threshold;
  return color;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  vec2 uv = (2.0 * fragCoord.xy - iResolution.xy) / iResolution.y;
    
  vec3 col = vec3(0.);
    
#ifdef AA 
  // Antialiasing via supersampling
  float uvs = 1. / max(iResolution.x, iResolution.y);
   
  for (float i = -AA; i < AA; ++i){
    for (float j = -AA; j < AA; ++j){
      col += renderMetaBall(uv + vec2(i, j) * (uvs / AA)) / (4.* AA * AA);
    }
  }
#else       
  col = renderMetaBall(uv);
#endif /* AA */
  fragColor = vec4(col,1.0);
}
