float dc_sin(float n);
float dc_cos(float n);
float dc_tan(float n);
float dc_cot(float n);

float dc_u16_sin(uint16 n);
float dc_u16_cos(uint16 n);
float dc_u16_tan(uint16 n);
float dc_u16_cot(uint16 n);

float dc_sqrt(float n);
float dc_isqrt(float n);

void dc_vector3_normalize(float *v);

#define dc_abs(n) ({float a=(n);asm("fabs %0":"=f"(a):"0"(a));a;})
