t_obj *init_one(void)
{
	t_obj *obj = calloc(1, sizeof(t_obj));
	return obj;
}

t_obj init_two(void)
{
	t_obj obj;
	return obj;
}

init_three(t_obj **obj)
{
	*obj = calloc(1, sizeof(t_obj));
}

int main()
{
	t_obj *first_obj = init_one();
	t_obj sec_obj = init_two();
	t_obj th_obj;
	init_three(&obj);
}
