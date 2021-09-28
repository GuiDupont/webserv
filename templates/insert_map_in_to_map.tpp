/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   insert_map_in_to_map.tpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 14:34:36 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/28 14:54:46 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INSERT_MAP
# define INSERT_MAP

# include <map>

template <class t, class t2>
void				insert_map_into_map(std::map<t, t2> origin, std::map<t, t2> dest) {
	for (typename std::map<t, t2>::iterator it = origin.begin(); it != origin.end(); it++)
		dest.insert(*it);
}

# endif