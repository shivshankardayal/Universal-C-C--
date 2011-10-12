#COMPILER=gcc -DICVM -DICVMTEST
#LINKER=gcc -L/usr/X11/lib

COMPILER=igcc
LINKER=igcc 


CC=$(COMPILER) -w  -DALL_IN_ONE  -DVERSION=\"IC-1.2\"  -O2  -c

	    

AR=iar -qc 




.c.o:
	${CC} $*.c

.cc.o:
	${CC} $*.cc

OBJ=\
am_map.o \
d_items.o \
d_main.o \
d_client.o \
d_deh.o \
doomdef.o \
doomstat.o \
dstrings.o \
f_finale.o \
f_wipe.o \
g_game.o \
hu_lib.o \
hu_stuff.o \
info.o \
l_joy.o \
l_main.o \
l_sound.o \
l_system.o \
l_video_trans.o \
l_video_x.o \
l_udp.o \
lprintf.o \
m_argv.o \
m_bbox.o \
m_menu.o \
m_misc.o \
m_random.o \
p_ceilng.o \
p_doors.o \
p_enemy.o \
p_floor.o \
p_genlin.o \
p_inter.o \
p_lights.o \
p_map.o \
p_maputl.o \
p_mobj.o \
p_plats.o \
p_pspr.o \
p_saveg.o \
p_setup.o \
p_sight.o \
p_spec.o \
p_switch.o \
p_telept.o \
p_tick.o \
p_user.o \
r_bsp.o \
r_data.o \
r_draw.o \
r_main.o \
r_plane.o \
r_segs.o \
r_sky.o \
r_things.o \
s_sound.o \
sounds.o \
st_lib.o \
st_stuff.o \
tables.o \
v_video.o \
version.o \
w_wad.o \
wi_stuff.o \
z_bmalloc.o \
z_zone.o \
m_cheat.o \


all: idoom

idoom: $(OBJ) 
	$(LINKER) $(OBJ)   -lXext \
		     -lX11  -lm  -o idoom 





clean:
	rm *.o  *.a

 