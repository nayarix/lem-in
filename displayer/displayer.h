#ifndef DISPLAYER_H
# include "libsdl/libsdl.h"
# include "../libft/includes/libft.h"

# define	WIDTH	800 //2200
# define	HEIGHT	600 //1200

# define	TITLE	"Ishobe Take Care of your ants"

# define	FONT_TYPE_TXT	"./libsdl/fonts/arial.ttf"
# define	FONT_SIZE_TXT	30

# define	IMG_PATH	"libsdl/Images/bk.jpg"

# define	L_WHITE -1
# define	L_BLACK 0
# define	L_RED	1
# define	L_GREEN 2
# define	L_YELLOW 3
# define	L_BLUE 4
# define	L_ORANGE 5
# define	L_CYAN 6
# define	L_MAGENTA 7

typedef struct		s_ant_infos
{
	int				*tab_life;
	int				out;
	int				color;
}					t_ant_infos;

typedef struct		s_room
{
	int				id;
	int				color;
	t_point			coord;
}					t_room;

typedef struct		s_infos
{
	t_chr			*input;
	int				ants;
	int				shots;
	int				v;
	int				start;
	int				end;
	char			**tab_hash;
	t_room			*rooms;
	int				**links;
	t_ant_infos		*tab_ants;
	int				color_paths;
	int				debug;
}					t_infos;

typedef struct		s_display
{
	t_sdlenv		env;
	t_infos			infos;
	SDL_Event		event;
	SDL_Color		color_text;
	TTF_Font		*font_text;
	t_point			offset;
	int				start_ants;
	int				end_ants;
	int				name_size;
	int				edge_size;
	int				moment;
	int				block;
	int				pause;
	int				pass;
	int				step;
}					t_display;

void		create_tabhash_2(t_infos *infos, t_chr *list);
void		take_cmds(t_infos *infos, t_chr *curr);
int			alloc_places(t_infos *infos);
int			the_color(char *str, int def);
void		free_infos(t_infos *infos);
void		free_error(t_infos *infos);
void		init_infos(t_infos *infos);
int			is_strnbr(char *str);
int			is_link(char *str);
int			is_room(char *str);
void		print_infos(t_infos infos);			  //!!!!!!!!!!
void		print_matrix(int **matrix, int size); //!!!!!!!!!!

#endif
