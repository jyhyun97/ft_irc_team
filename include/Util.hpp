/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeonhyun <jeonhyun@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 16:26:51 by swang             #+#    #+#             */
/*   Updated: 2022/06/03 16:28:43 by jeonhyun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include <iostream>


std::string appendStringColon(size_t startIndex, std::vector<std::string> msg);
std::vector<std::string> split(std::string &line, std::string s);
void print_stringVector(std::vector<std::string> v);

#endif
