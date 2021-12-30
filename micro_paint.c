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
	float x;
	float y;
	float height;
	float width;
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
	ret->width = src.width;
	ret->height = src.height;
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

	res = fscanf(fd, "%d %d %c\n", &(field->width), &(field->height), &(field->b_char));
	if(field->width <= 0 || field->width > 300 ||
		field->height <= 0 || field->height > 300 ||
		res != 3)
	{
		return(1);
	}
	return(0);
}

int get_circles(FILE *fd, l_list **lst)
{
	l_list l;
	int res;

	res = fscanf(fd, "%c %f %f %f %f %c\n",
		&(l.type), &(l.x), &(l.y), &(l.width), &(l.height), &(l.c_char));
	if(res == -1)
		return(0);
	else if(res != 6 || (l.type != 'r' && l.type != 'R') || l.width <= 0 || l.height <= 0)
		return(1);
	l_list *start = lst_new(l);
	if(!start)
		return(1);
	while((res = fscanf(fd, "%c %f %f %f %f %c\n",
		&(l.type), &(l.x), &(l.y), &(l.width), &(l.height), &(l.c_char))) == 6)
	{
		if((l.type != 'r' && l.type != 'R') || l.width <= 0 || l.height <= 0)
		{
			res = 0;
			break;
		}
		lst_add(start, lst_new(l));
	}
	*lst = start;
	if(res != -1)
	{
		return(1);
	}
	return(0);
}

float sqr(float x)
{
	return(x * x);
}

int get_char(l_list *lst, float x, float y)
{
	if (((x < lst->x || (lst->x + lst->width < x))
		|| (y < lst->y)) || (lst->y + lst->height < y))
		return (0);
	if (((x - lst->x < 1) || ((lst->x + lst->width) - x < 1)) ||
	((y - lst->y < 1 || ((lst->y + lst->height) - y < 1))))
		return (2);
	return (1);
}

int check_prox(l_list *lst, float y, float x)
{
	l_list *ptr = lst;
	int ret = 0, buff;
	while(ptr)
	{
		buff = get_char(ptr, x, y);
		if((buff == 2 && lst->type == 'r') ||
			(lst->type == 'R' && buff))
			ret = lst->c_char;
		buff = 0;
		ptr = ptr->next;
	}
	return(ret);
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
			c = check_prox(lst, (float)height, (float)width);
			if(c)
				arr[height][width] = c;
			c = 0;
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
	lst = NULL;
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
