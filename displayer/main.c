/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obelouch <OB-96@hotmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/05 01:41:34 by obelouch          #+#    #+#             */
/*   Updated: 2019/07/22 07:05:20 by obelouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "displayer.h"

static void		find_start_end(t_chr *curr, char *str)
{
	if (!ft_strcmp(str, "start"))
		curr->next->len = INT_MAX;
	else if (!ft_strcmp(str, "end"))
		curr->next->len = UINT_MAX;
}

void			fill_basic_infos(t_infos *infos)
{
	t_chr		*curr;	
	char		*str;

	curr = infos->input;
	if (!ft_strncmp(curr->str, "ERROR", 5))
		free_error(infos);
	while (curr)
	{
		str = curr->str;
		if (str[0] == '#')
		{
			if (str[1] != '#')
				ft_putendl(&str[1]);
			else
				find_start_end(curr, &str[2]);
		}
		else if (is_strnbr(str) && !infos->ants)
			infos->ants = ft_atoi(str);
		else if (is_room(str))
			infos->v++;
		else if (str[0] == 'L')
			infos->shots++;
		curr = curr->next;
	}
}

void			fill_room(t_infos *infos, char *str)
{
	char	**tab;
	int		ind;

	tab = ft_strsplit(str, ' ');
	ind = hash_findid(infos->tab_hash, infos->v, tab[0]);
	infos->rooms[ind].id = ind;
	infos->rooms[ind].coord.x = ft_atoi(tab[1]);
	infos->rooms[ind].coord.y = ft_atoi(tab[2]);
	free_tabstr(&tab);
}

void			add_link(char **tab_hash, int v, int **matrix, char *str)
{
	char		**tab;
	int			a;
	int			b;

	tab = ft_strsplit(str, '-');
	a = hash_findid(tab_hash, v, tab[0]);
	b = hash_findid(tab_hash, v, tab[1]);
	matrix[a][b] = 1;
	matrix[b][a] = 1;
}

static int		modify_l(char **str)
{
	char		*s;
	int			i;

	i = 1;
	s = *str;
	while(s[i] && s[i] != '-')
		i++;
	(*str)[i] = '\0';
	return (i);
}

void			fill_tab_ants(t_infos *infos, t_ant_infos *tab_ants, char *str, int time)
{
	char		**tab_cmds;
	int			num_ant;
	int			id_room;
	int			eol;
	int			i;

	i = 0;
	tab_cmds = ft_strsplit(str, ' ');
	while (tab_cmds[i])
	{
		eol = modify_l(&tab_cmds[i]);
		num_ant = ft_atoi(&(tab_cmds[i][1]));
		id_room = hash_findid(infos->tab_hash, infos->v, &tab_cmds[i][eol + 1]);
		tab_ants[num_ant - 1].tab_life[time] = id_room;
		i++;
	}
	free_tabstr(&tab_cmds);
}

void			fill_adv_infos(t_infos *infos)
{
	t_chr		*curr;
	int			time;

	curr = infos->input;
	while (curr)
	{
		if (!ft_strncmp(curr->str, "##", 2))
			take_cmds(infos, curr);
		else if (is_room(curr->str))
			fill_room(infos, curr->str);
		else if (is_link(curr->str))
			add_link(infos->tab_hash, infos->v, infos->links, curr->str);
		else if (curr->str[0] == 'L')
			break ;
		curr = curr->next;
	}
	time = 0;
	while (curr)
	{
		fill_tab_ants(infos, infos->tab_ants, curr->str, time);
		curr = curr->next;
		time++;
	}
}

void			take_options(int ac, char **av, t_infos *infos)
{
	int			i;

	i = 0;
	while (++i < ac)
	{
		if (!ft_strcmp(av[i], "-d"))
			infos->debug = 1;
		else
		{
			free_infos(infos);
			ft_dprintf(2, "Usage: ./test [-d]\n");
			ft_dprintf(2, "  -d : enable debug mode\n");
			exit(EXIT_FAILURE);
		}
	}
}
/*
static void		print_infos_tabhash(char **tab, int v)
{
	int		i = 0;

	ft_printf("dsd\n");
	while (i < v)
	{
		ft_printf("tab[%d] = %s\n", i, tab[i]);
		i++;
	}
}
*/
static void		print_infos_rooms(t_room *rooms, int size)
{
	int			i;

	i = 0;
	ft_printf("*%{cyan}informations about rooms%{eoc}:\n");
	while (i < size)
	{
		ft_printf("%{yellow}======| %{eoc}room %d%{yellow} |======%{eoc}\n", rooms[i].id);
		ft_printf("   color: %d\n", rooms[i].color);
		ft_printf("   coord: (x = %d, y = %d)\n", rooms[i].coord.x, rooms[i].coord.y);
		ft_putchar('\n');
		i++;
	}
}

void			print_debug(t_infos infos)
{
	//print_infos_tabhash(infos.tab_hash, infos.v);
	ft_printf("We have %{RED}%d%{eoc} ant.\n", infos.ants);
	ft_printf("The Graph have %{GREEN}%d%{eoc} vertex.\n", infos.v);
	ft_printf("start: [%{RED}%d%{eoc}] | ", infos.start);
	ft_printf("end: [%{RED}%d%{eoc}]\n", infos.end);
	print_infos_rooms(infos.rooms, infos.v);
	//print_adj_matrix(infos.links, infos.v);
	//print_life_ants(infos.tab_ants, infos.ants, infos.shots);
	ft_printf("The Goal is done in %{RED}%d%{eoc} instruction\n", infos.shots);
}

int				store_data(int ac, char **av, t_infos *infos)
{
	init_infos(infos);
	take_options(ac, av, infos);
	if (!(infos->input = gnl_save_chr(0)))
		return(0);
	chr_print(infos->input);
	ft_putstr("\n\n");
	fill_basic_infos(infos);
	if (!alloc_places(infos))
	{
		free_infos(infos);
		exit(0);
	}
	create_tabhash_2(infos, infos->input);
	fill_adv_infos(infos);
	if (infos->debug)
		print_debug(*infos);
		//print_infos(*infos);		//!!!!!!!!!!!!!!!!!!
	return (1);
}

void			print_usage(void)
{
	ft_putstr("Welcome\n");
}

void			init_vars_display(t_display *display)
{
	int			i;

	i = 0;
	while (i < display->infos.ants)
		display->infos.tab_ants[i++].out = 0;
	display->start_ants = display->infos.ants;
	display->end_ants = 0;
	display->moment = 1;
	display->step = 1;
	display->pause = 1;
	display->pass = 1;
	display->block = WIDTH / 10;
	display->color_text = setcolor_sdl(0, 0, 0, 0);
	display->offset = ft_setpoint(400, 200);
	display->name_size = 18;
	display->edge_size = display->block / 8;
}

int				init_display(t_display *display)
{
	init_sdlenv(&(display->env), 1, 1, 0);
	init_sdl(display->env);
	if (!fill_sdlenv(&(display->env), TITLE, HEIGHT, WIDTH))
		return (0);
	//load_music(display->env, MUSIC_PATH);
	display->font_text = TTF_OpenFont(FONT_TYPE_TXT, FONT_SIZE_TXT);
	init_vars_display(display);
	return (1);
}

static char		*str_msg(char *msg, int nbr)
{
	char		*str;
	char		*tmp;
	
	str = ft_strdup(msg);
	tmp = ft_itoa(nbr);
	ft_strcombin(&str, tmp);
	free(tmp);
	return (str);
}

void			display_shots(t_display *display, int shots)
{
	SDL_Texture	*tex;
	SDL_Rect	pos;
	char		*str;

	str = str_msg("Shots: ", shots);
	//ft_strcombin(&str, " shot");
	tex = ttf_texture(display->env.render, display->font_text,
						str, display->color_text);
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	pos.y = HEIGHT / 10;
	pos.x = WIDTH / 30 + WIDTH / 10 - pos.w / 2;
	SDL_RenderCopy(display->env.render, tex, NULL, &pos);
	SDL_DestroyTexture(tex);
	free(str);
}

void			display_ants(t_display *display, int start_ants, int end_ants)
{
	SDL_Texture	*tex;
	SDL_Rect	pos;
	char		*str;

	str = str_msg("Start: ", start_ants);
	tex = ttf_texture(display->env.render, display->font_text,
						str, display->color_text);
	pos = create_rect(0, 0, HEIGHT / 20, WIDTH / 20);
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(display->env.render, tex, NULL, &pos);
	SDL_DestroyTexture(tex);
	str = str_msg("End: ", end_ants);
	tex = ttf_texture(display->env.render, display->font_text,
						str, display->color_text);
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	pos.y = HEIGHT / 20;
	pos.x = WIDTH / 60 + WIDTH / 5 - pos.w;
	SDL_RenderCopy(display->env.render, tex, NULL, &pos);
	SDL_DestroyTexture(tex);
}

SDL_Color		color_macros(int macros)
{
	if (macros == L_YELLOW)
		return (setcolor_sdl(255, 255, 0, 1));
	if (macros == L_WHITE)
		return (setcolor_sdl(255, 255, 255, 1));
	if (macros == L_BLACK)
		return (setcolor_sdl(0, 0, 0, 1));
	if (macros == L_GREEN)
		return (setcolor_sdl(0, 255, 0, 1));
	if (macros == L_RED)
		return (setcolor_sdl(255, 0, 0, 1));
	if (macros == L_BLUE)
		return (setcolor_sdl(0, 0, 255, 1));
	if (macros == L_ORANGE)
		return (setcolor_sdl(255, 70, 0, 1));
	if (macros == L_CYAN)
		return (setcolor_sdl(0, 255, 255, 1));
	if (macros == L_MAGENTA)
		return (setcolor_sdl(255, 0, 255, 1));
	return (setcolor_sdl(0, 0, 0, 1));
}

static void		draw_link(t_display *display, t_infos infos, int a, int b)
{
	t_room		*rooms;
	t_point		p_a;
	t_point		p_b;
	t_bline		bold;
	int			i;
	int			j;
	int			k;

	k = 0;
	rooms = infos.rooms;
	while (k < infos.v)
	{
		if (rooms[k].id == a)
			i = k;
		if (rooms[k].id == b)
			j = k;
		k++;
	}
	p_a = ft_setpoint(display->offset.y + rooms[i].coord.y * display->block,
					display->offset.x + rooms[i].coord.x * display->block);
	p_b = ft_setpoint(display->offset.y + rooms[j].coord.y * display->block,
					display->offset.x + rooms[j].coord.x * display->block);
	bold = ft_setboldline(p_a, p_b, display->edge_size);
	drawboldline_sdl(display->env, color_macros(infos.color_paths), bold);
}

static void		draw_edge(t_display *display)
{
	int			**matrix;
	int			size;
	int			i;
	int			j;

	i = 0;
	size = display->infos.v;
	matrix = display->infos.links;
	while (i < size)
	{
		j = 0;
		while (j < size)
		{
			if (matrix[i][j] == 1)
				draw_link(display, display->infos, i, j);
			j++;
		}
		i++;
	}
}

static void		write_limits(t_display *display, int is_start, t_point c, int r)
{
	SDL_Texture	*texture;
	TTF_Font	*font;
	SDL_Rect	pos;
	char		*name;

	font = TTF_OpenFont(FONT_TYPE_TXT, display->name_size);
	name = (is_start) ? "start" : "end";
	texture = ttf_texture(display->env.render, font, name, setcolor_sdl(0, 0, 0, 1));
	SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
	pos.y = c.y + display->block / 8;
	pos.x = c.x - pos.w / 2;
	SDL_RenderCopy(display->env.render, texture, NULL, &pos);
	SDL_DestroyTexture(texture);
	TTF_CloseFont(font);
}

static void		limits_square(t_display *display, t_point c, int r, int is_start)
{
	SDL_Color	colors[2];

	colors[0] = setcolor_sdl(0, 0, 0, 1);
	if (is_start)
		colors[1] = setcolor_sdl(0, 0, 255, 1);
	else
		colors[1] = setcolor_sdl(0, 255, 0, 1);
	drawfillsquare_sdl_c(display->env, colors, c, 2 * r);
	write_limits(display, is_start, c, r);
}

static void		drawnormal_disk(t_sdlenv env, SDL_Color color, t_point c, int r)
{
	SDL_Color	black;

	black = setcolor_sdl(0, 0, 0, 1);
	drawdisk_sdl(env, color, c, r);
	drawcircle_sdl(env, black, c, r);
}

static void		write_name(t_display *display, char *name, t_point c, int r)
{
	SDL_Texture	*texture;
	TTF_Font	*font;
	SDL_Rect	pos;

	font = TTF_OpenFont(FONT_TYPE_TXT, display->name_size);
	texture = ttf_texture(display->env.render, font, name, setcolor_sdl(0, 0, 0, 1));
	SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
	pos.y = c.y - display->block / 4;
	pos.x = c.x - pos.w / 2;
	SDL_RenderCopy(display->env.render, texture, NULL, &pos);
	SDL_DestroyTexture(texture);
	TTF_CloseFont(font);
}

static void		draw_rooms(t_display *display)
{
	t_infos		infos;
	SDL_Color	colors[2];
	t_point		c;
	int			r;
	int			i;

	i = 0;
	r = display->block / 3;
	infos = display->infos;
	colors[0] = setcolor_sdl(0, 0, 0, 1);
	while (i < infos.v)
	{
		colors[1] = color_macros(infos.rooms[i].color);
		c = ft_setpoint(display->offset.y + infos.rooms[i].coord.y * display->block,
					display->offset.x + infos.rooms[i].coord.x * display->block);
		if (infos.rooms[i].id == infos.start)
			limits_square(display, c, r, 1);
		else if (infos.rooms[i].id == infos.end)
			limits_square(display, c, r, 0);
		else
			drawnormal_disk(display->env, colors[1], c, r);
		write_name(display, infos.tab_hash[infos.rooms[i].id], c, r);
		i++;
	}
}

void			display_graph(t_display *display)
{
	draw_edge(display);
	draw_rooms(display);
}

void			display_vars(t_display *display, int start_ants, int end_ants, int shots)
{
	display_shots(display, shots);
	display_ants(display, start_ants, end_ants);
}

void			draw_scene(t_display *display)
{
	SDL_Texture		*texture;
	SDL_Rect		rect;

	texture = texture_img(display->env.render, IMG_PATH);
	SDL_RenderCopy(display->env.render, texture, NULL, NULL);
	SDL_DestroyTexture(texture);
	rect = create_rect(HEIGHT / 8, WIDTH / 5, HEIGHT / 30, WIDTH / 30);
	SDL_SetRenderDrawColor(display->env.render, 160, 160, 160, 255);
	SDL_RenderFillRect(display->env.render, &rect);
	display_graph(display);
}

static void		draw_num_ant(t_display *display, t_point coord_ant, int x)
{
	SDL_Texture	*num;
	TTF_Font	*font;
	char		*nbr;
	SDL_Rect	pos;

	nbr = ft_itoa(x);
	font = TTF_OpenFont(FONT_TYPE_TXT, display->block / 18);
	num = ttf_texture(display->env.render, font, nbr, setcolor_sdl(100, 100, 100, 1));
	SDL_QueryTexture(num, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(display->env.render, num, NULL, &pos);
	pos.x = coord_ant.x;
	pos.y = coord_ant.y;
	SDL_DestroyTexture(num);
	TTF_CloseFont(font);
	free(nbr);
}

void			draw_ant(t_display *display, t_infos infos, int x)
{
	t_point		coord_ant;
	SDL_Color	color;
	int			v_now;

	v_now = infos.tab_ants[x].tab_life[display->moment - 1];
	color = color_macros(infos.tab_ants[x].color);
	coord_ant = ft_setpoint(display->offset.y + infos.rooms[v_now].coord.y * display->block,
					display->offset.x + infos.rooms[v_now].coord.x * display->block);
	drawdisk_sdl(display->env, color, coord_ant, display->block / 15);
	drawcircle_sdl(display->env, setcolor_sdl(0, 0, 0, 1), coord_ant, display->block / 15);
	draw_num_ant(display, coord_ant, x);
}

void			draw_state(t_display *display, t_infos infos)
{
	int			ant_state;
	int			i;

	i = 0;
	draw_scene(display);
	while (i < infos.ants)
	{
		ant_state = infos.tab_ants[i].tab_life[display->moment - 1];
		if (ant_state != -1)
		{
			if (!infos.tab_ants[i].out)
			{
				display->start_ants--;
				infos.tab_ants[i].out = 1;
			}
			if (ant_state == infos.end)
				display->end_ants++;
			draw_ant(display, infos, i);
		}
		i++;
	}
	display_vars(display, display->start_ants, display->end_ants, display->moment);
	SDL_RenderPresent(display->env.render);
}


void			draw_full_limit(t_display *display, int is_start)
{
	t_infos		infos;
	t_point		coord_ant;
	SDL_Color	color;

	infos = display->infos;
	color = setcolor_sdl(0, 0, 0, 1);
	if (is_start)
		coord_ant = ft_setpoint(display->offset.y + infos.rooms[infos.start].coord.y * display->block,
					display->offset.x + infos.rooms[infos.start].coord.x * display->block);
	else
		coord_ant = ft_setpoint(display->offset.y + infos.rooms[infos.end].coord.y * display->block,
					display->offset.x + infos.rooms[infos.end].coord.x * display->block);
	drawdisk_sdl(display->env, color, coord_ant, display->block / 15);
}

static void		event_zoom(t_display *display)
{
	if (display->event.key.keysym.sym == SDLK_KP_PLUS &&
		display->block < WIDTH / 5)
	{
		display->block += 10;
		display->offset.x -= 20;
		display->edge_size = display->block / 8;
	}
	if (display->event.key.keysym.sym == SDLK_KP_MINUS &&
		display->block > WIDTH / 50)
	{
		display->block -= 10;
		display->offset.x += 20;
		display->edge_size = display->block / 8;
	}
}

void			event_keydown(t_display *display)
{
	if (display->event.key.keysym.sym == SDLK_ESCAPE)
		display->pass = 0;
	else if (display->event.key.keysym.sym == SDLK_SPACE)
		display->pause = (display->pause) ? 0 : 1;
	else if (display->event.key.keysym.sym == SDLK_RIGHT)
		display->offset.x += 20;
	else if (display->event.key.keysym.sym == SDLK_LEFT)
		display->offset.x -= 20;
	else if (display->event.key.keysym.sym == SDLK_UP)
		display->offset.y -= 20;
	else if (display->event.key.keysym.sym == SDLK_DOWN)
		display->offset.y += 20;
	else if (display->event.key.keysym.sym == SDLK_r)	
		init_vars_display(display);
	else if (display->event.key.keysym.sym == SDLK_KP_PLUS ||
		display->event.key.keysym.sym == SDLK_KP_MINUS)
		event_zoom(display);
}

void			free_display(t_display *display)
{
	TTF_CloseFont(display->font_text);
	free_infos(&(display->infos));
	free_sdl(&(display->env));
}

void			draw_fix_scene(t_display display)
{
	draw_scene(&display);
	display_vars(&display, display.start_ants,
	display.end_ants, ft_min(display.moment, display.infos.shots));
	if (display.start_ants > 0)
		draw_full_limit(&display, 1);
	if (display.end_ants > 0)
		draw_full_limit(&display, 0);
	SDL_RenderPresent(display.env.render);
}

int				main(int ac, char **av)
{
	t_display	display;

	print_usage();
	if (!store_data(ac, av, &(display.infos)))
		return(EXIT_FAILURE);
	if (!init_display(&display))
		return(EXIT_FAILURE);
	while (display.pass)
	{
		while (SDL_PollEvent(&(display.event)))
		{
			if (display.event.type == SDL_QUIT)
				display.pass = 0;
			if (display.event.type == SDL_KEYDOWN)
				event_keydown(&display);
		}
		if (!display.pause && display.moment <= display.infos.shots)
		{
			draw_state(&display, display.infos);
			display.moment++;
			SDL_Delay(200);
		}
		else
			draw_fix_scene(display);
	}
	free_display(&display);
	return (EXIT_SUCCESS);
}
