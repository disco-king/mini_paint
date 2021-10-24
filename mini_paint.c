#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct fld{
	int width;
	int height;
	char b_char;
} t_field;

typedef struct s_list{
	char type;
	double x;
	double y;
	double radius;
	char c_char;
	struct s_list *next;
} l_list;

void ft_putstr(const char *str)
{
	int i = 0;
	while(str[i])
	{
		write(1, &(str[i]), 1);
		i++;
	}
}

void free_lst(l_list *lst)
{
	if(!lst)
		return ;
	l_list *buff = lst;
	lst = lst->next;
	while(buff->next)
	{
		free(buff);
		buff = lst;
		lst = lst->next;
	}
	free(buff);
}

l_list *lst_new(l_list src)
{
	l_list *ret = (l_list *)malloc(sizeof(l_list));
	if(!ret)
		return(ret);
	ret->type = src.type;
	ret->x = src.x;
	ret->y = src.y;
	ret->radius = src.radius;
	ret->c_char = src.c_char;
	ret->next = NULL;
	return(ret);
}

void lst_add(l_list *lst, l_list *new)
{
	l_list *ptr = lst;
	while(ptr->next)
	{
		ptr = ptr->next;
	}
	ptr->next = new;
}

void free_arr(char **arr)
{
	char **ptr = arr;
	while(*arr)
	{
		free(*arr);
		arr++;
	}
	free(*arr);
	free(ptr);
}

char **get_arr(t_field *field)
{
	char **arr = (char **)malloc((field->height + 1) * sizeof(char *));
	arr[field->height] = NULL;
	int i = 0;
	while(i < field->height)
	{
		arr[i] = (char *)malloc((field->width + 1) * sizeof(char));
		memset(arr[i], field->b_char, field->width);
		arr[i][field->width] = 0;
		i++;
	}
	return(arr);
}

int get_field(t_field *field, FILE *fd)
{
	int res;

	res = fscanf(fd, "%d %d %c", &(field->width), &(field->height), &(field->b_char));
	if(field->width <= 0 || field->width > 300 ||
		field->height <= 0 || field->height > 300 ||
		res != 3)
		return(1);
	return(0);
}

int get_circles(FILE *fd, l_list **lst)
{
	l_list l;
	int res;

	if((res = fscanf(fd, "\n%c %lf %lf %lf %c",
		&(l.type), &(l.x), &(l.y), &(l.radius), &(l.c_char))) != 5)
	{
		if(res == -1)
		{
			*lst = NULL;
			return(0);
		}
		return(1);
	}
	l_list *start = lst_new(l);
	if(!start)
		return(1);
	while((res = fscanf(fd, "\n%c %lf %lf %lf %c",
		&(l.type), &(l.x), &(l.y), &(l.radius), &(l.c_char))) == 5)
	{
		lst_add(start, lst_new(l));
	}
	*lst = start;
	if(res != -1)
		return(1);
	return(0);
}

double sqr(double x)
{
	return(x * x);
}

int check_prox(l_list *lst, double y, double x)
{
	l_list *ptr = lst;
	double dist;
	int flag = 0;
	double type;
	while(ptr)
	{
		if(ptr->type == 'C')
			type = 0;
		else
			type = 1;
		dist = sqrtf(sqr(ptr->x - x) + sqr(ptr->y - y));
		if(type == 1)
		{
			if(dist > ptr->radius - type && dist <= ptr->radius)
				flag = ptr->c_char;
		}
		else
			if(dist <= ptr->radius)
				flag = ptr->c_char;
		ptr = ptr->next;
	}
	return(flag);
}

void fill_arr(char **arr, l_list *lst)
{
	int height = 0;
	int width;
	int c;

	if(lst == NULL)
		return ;
	while(arr[height])
	{
		width = 0;
		while(arr[height][width])
		{
			c = check_prox(lst, (double)height, (double)width);
			if(c)
				arr[height][width] = c;
			width++;
		}
		height++;
	}
}

int exitf(int code)
{
	if(code)
		ft_putstr("Error: Operation file corrupted\n");
	else
		ft_putstr("Error: argument\n");
	return(1);
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		return(exitf(0));
	}
	FILE *fd = fopen(argv[1], "r");
	if(!fd)
	{
		return(exitf(1));
	}
	t_field *field = (t_field *)malloc(sizeof(t_field));
	if(get_field(field, fd))
	{
		free(field);
		fclose(fd);
		return(exitf(1));
	}
	l_list *lst;
	if(get_circles(fd, &lst))
	{
		free_lst(lst);
		free(field);
		fclose(fd);
		return(exitf(1));
	}
	char **field_arr = get_arr(field);
	fill_arr(field_arr, lst);
	char **buff = field_arr;
	while(*buff)
	{
		ft_putstr(*buff);
		ft_putstr("\n");
		buff++;
	}
	free_arr(field_arr);
	fclose(fd);
	free_lst(lst);
	free(field);
	return(0);
}
