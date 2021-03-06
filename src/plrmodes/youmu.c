/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2017, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2017, Andrei Alexeyev <akari@alienslab.net>.
 */

#include "global.h"
#include "plrmodes.h"

PlayerCharacter character_youmu = {
    .id = PLR_CHAR_YOUMU,
    .lower_name = "youmu",
    .proper_name = "Yōmu",
    .full_name = "Konpaku Yōmu",
    .title = "Half-Phantom Girl",
    .dialog_sprite_name = "dialog/youmu",
    .player_sprite_name = "youmu",
    .ending = {
        .good = good_ending_youmu,
        .bad = bad_ending_youmu,
    },
};

int youmu_common_particle_spin(Projectile *p, int t) {
    int i = timeout_linear(p, t);

    if(t < 0)
        return 1;

    p->args[3] += 0.06;
    p->angle = p->args[3];

    return i;
}

void youmu_common_particle_slice_draw(Projectile *p, int t) {
    if(t < creal(p->args[0])/20.0)
        p->args[1] += 1;

    if(t > creal(p->args[0])-10) {
        p->args[1] += 3;
        p->args[2] += 1;
    }

    float f = p->args[1]/p->args[0]*20.0;

    glColor4f(1,1,1,1.0 - p->args[2]/p->args[0]*20.0);

    glPushMatrix();
    glTranslatef(creal(p->pos), cimag(p->pos),0);
    glRotatef(p->angle,0,0,1);
    glScalef(f,1,1);
    draw_texture(0,0,"part/youmu_slice");

    glPopMatrix();

    glColor4f(1,1,1,1);
}

void youmu_common_shot(Player *plr) {
    if(!(global.frames % 4)) {
        play_sound("generic_shot");
    }

    if(!(global.frames % 6)) {
        create_projectile1c("youmu", plr->pos + 10 - I*20, 0, linear, -20.0*I)->type = PlrProj+120;
        create_projectile1c("youmu", plr->pos - 10 - I*20, 0, linear, -20.0*I)->type = PlrProj+120;
    }
}
