/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmomeni <mmomeni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 15:15:08 by htaheri           #+#    #+#             */
/*   Updated: 2024/03/28 22:58:07 by mmomeni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_quadratic	solve_quadratic(float a, float b, float c)
{
	t_quadratic	q;

	q.delta = b * b - 4 * a * c;
	if (q.delta < 0)
		return (q);
	q.t1 = (-b + sqrt(q.delta)) / (2 * a);
	q.t2 = (-b - sqrt(q.delta)) / (2 * a);
	return (q);
}

t_color	color_scale(t_color color, float scale)
{
	t_color	scaled_color;

	scaled_color = (t_color){color.r * scale, color.g * scale, color.b * scale};
	return (scaled_color);
}

static int	ray_hit_sphere(t_ray *ray, t_sphere sphere)
{
	t_vec3		oc;
	t_quadratic	q;

	oc = vec3_op(SUB, ray->o, sphere.pos);
	q.a = vec3_dot(ray->dir, ray->dir);
	q.b = 2.0 * vec3_dot(oc, ray->dir);
	q.c = vec3_dot(oc, oc) - (sphere.radius * sphere.radius);
	q = solve_quadratic(q.a, q.b, q.c);
	if (q.delta < 0 || (q.t1 < 0 && q.t2 < 0))
		return (0);
	else if (q.t1 < 0)
		ray->t = q.t2;
	else if (q.t2 < 0)
		ray->t = q.t1;
	else
		ray->t = fmin(q.t1, q.t2);
	return (1);
}

static int	ray_hit_plane(t_ray *ray, t_plane plane)
{
	float	denom;
	float	t;

	denom = vec3_dot(plane.normal, ray->dir);
	if (fabs(denom) < 0.00001)
		return (0);
	t = vec3_dot(vec3_op(SUB, plane.pos, ray->o), plane.normal) / denom;
	if (t < 0)
		return (0);
	ray->t = t;
	return (1);
}

t_object	*ray_get_hit(t_scene *scene, t_ray *ray)
{
	float	t_min;
	size_t	i;
	int		j;

	t_min = INFINITY;
	i = -1;
	j = -1;
	while (++i < scene->obj_count)
	{
		if (scene->objects[i].type == SPHERE)
			ray_hit_sphere(ray, scene->objects[i].sphere);
		else if (scene->objects[i].type == PLANE)
			ray_hit_plane(ray, scene->objects[i].plane);
		else if (scene->objects[i].type == CYLINDER)
			ray_hit_cyl(ray, scene->objects[i].cylinder);
		if (ray->t < t_min)
		{
			t_min = ray->t;
			j = i;
		}
	}
	ray->t = t_min;
	if (j == -1)
		return (NULL);
	return (&scene->objects[j]);
}
