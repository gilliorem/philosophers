/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 16:06:18 by regillio          #+#    #+#             */
/*   Updated: 2026/01/30 06:09:06 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philos.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*result;

	if (nmemb == 0 || size == 0)
		return (malloc(0));
	if (nmemb && size && SIZE_MAX / nmemb < size)
		return (NULL);
	result = malloc(nmemb * size);
	if (!result)
		return (NULL);
	memset(result, 0, nmemb * size);
	return (result);
}

int	ft_atoi(const char *nptr)
{
	int	minus_count;
	int	i;
	int	result;

	minus_count = 0;
	i = 0;
	result = 0;
	while ((nptr[i] == ' ') || ((nptr[i]) >= 9 && nptr[i] <= 13))
		i++;
	while (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			minus_count++;
		i++;
		break ;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + nptr[i] - '0';
		i++;
	}
	if (minus_count > 0)
		result = result * (-1);
	return (result);
}
