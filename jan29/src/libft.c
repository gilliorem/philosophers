/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 16:06:18 by regillio          #+#    #+#             */
/*   Updated: 2026/01/29 16:13:15 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philos.h"


void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*result;

	if (nmemb == 0 || size == 0)
		return (malloc(0));
	if (nmemb && size && 99999999 / nmemb < size)
		return (NULL);
	result = malloc(nmemb * size);
	if (!result)
		return (NULL);
	memset(result, 0, nmemb * size);
	return (result);
}
