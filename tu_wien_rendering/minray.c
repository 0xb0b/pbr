/* minimal ray tracer, hybrid version - 888 tokens
 * Paul Heckbert, ucbvax!pixar!ph, 13 Jun 87
 * Using tricks from Darwyn Peachey and Joe Cychosz. */

#define TOL 1e-7
#define AMBIENT vec U, black, amb
#define SPHERE struct sphere {vec center, color; double rad, kd, ks, kt, kl, ir} \
    *sphere_ptr, *best, spheres[]
typedef struct vec {double x, y, z} vec;

#include "ray.h"
#include <math.h>
#include <stdio.h>


typedef struct Ray {
  vec o;
  vec d;
} Ray;


int yx;
double u, b, tmin;

double vdot(A, B)
vec A, B;
{
    return A.x*B.x + A.y*B.y + A.z*B.z;
}

vec vcomb(a, A, B)  /* aA+B */
double a;
vec A, B;
{
    B.x += a*A.x;
    B.y += a*A.y;
    B.z += a*A.z;
    return B;
}

vec vunit(A)
vec A;
{
    return vcomb(1./sqrt(vdot(A, A)), A, black);
}

struct sphere *intersect(ray_origin, ray_direction)
vec ray_origin, ray_direction;
{
  best = 0;
  tmin = 1e30;
  sphere_ptr = spheres + NSPHERE;
  while (sphere_ptr-- > spheres)
    b = vdot(ray_direction, U = vcomb(-1., ray_origin, sphere_ptr->center)),
      u = b*b-vdot(U, U)+sphere_ptr->rad*sphere_ptr->rad,
      u = u>0 ? sqrt(u) : 1e31,
      u = b-u>TOL ? b-u : b+u,
      tmin = u>=TOL && u<tmin ?
        best = sphere_ptr, u : tmin;
  return best;
}

vec trace(level, ray_origin, ray_direction)
vec ray_origin, ray_direction;
{
  double d, eta, e;
  vec N, color;
  struct sphere *s, *l;

  if (level == 0) return black;
  level--;

  s = intersect(ray_origin, ray_direction);
  if (!s) return amb;

  color = amb;
  eta = s->ir;
  // ray origin moves to intersection point
  ray_origin = vcomb(tmin, ray_direction, ray_origin);
  N = vunit(vcomb(-1., ray_origin, s->center));
  d = -vdot(ray_direction, N);
  if (d<0)
    N = vcomb(-1., N, black),
      eta = 1/eta,
      d = -d;
  l = spheres+NSPHERE;
  while (l-->spheres)
    if ((e = l->kl*vdot(N, U = vunit(vcomb(-1., ray_origin, l->center)))) > 0 &&
        intersect(ray_origin, U)==l)
      color = vcomb(e, l->color, color);
  U = s->color;
  color.x *= U.x;
  color.y *= U.y;
  color.z *= U.z;
  e = 1-eta*eta*(1-d*d);
  /* the following is non-portable: we assume right to left arg evaluation.
   * (use U before call to trace, which modifies U) */
  return vcomb(s->kt,
      e>0 ? trace(level, ray_origin, vcomb(eta, ray_direction, vcomb(eta*d-sqrt(e), N, black)))
      : black,
      vcomb(s->ks, trace(level, ray_origin, vcomb(2*d, N, ray_direction)),
        vcomb(s->kd, color, vcomb(s->kl, U, black))));
}

main()
{
  printf("P3\n");
  printf("%d %d\n", SIZE, SIZE);
  printf("255\n");
  while (yx<SIZE*SIZE)
    U.x = yx%SIZE-SIZE/2,
      U.z = SIZE/2-yx++/SIZE,
      U.y = SIZE/2/tan(AOV/114.5915590261),  /* 360/PI~=114 */
      U = vcomb(255., trace(DEPTH, black, vunit(U)), black),
      printf("%.0f %.0f %.0f\n", U.x, U.y, U.z);  /* yowsa! non-portable! */
}
