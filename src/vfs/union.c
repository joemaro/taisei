/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2017, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2017, Andrei Alexeyev <akari@alienslab.net>.
 */

#include "union.h"

static bool vfs_union_mount_internal(VFSNode *unode, const char *mountpoint, VFSNode *mountee, VFSInfo info, bool seterror);

static void vfs_union_delete_callback(void **list, void *elem) {
    ListContainer *c = elem;
    VFSNode *n = c->data;
    vfs_decref(n);
    delete_element(list, elem);
}

static void vfs_union_free(VFSNode *node) {
    delete_all_elements((void**)&node->vunion.members, vfs_union_delete_callback);
}

static VFSNode* vfs_union_locate(VFSNode *node, const char *path) {
    VFSNode *u = vfs_alloc(true);
    vfs_union_init(u); // uniception!

    VFSInfo prim_info = VFSINFO_ERROR;
    ListContainer *first = node->vunion.members.all;
    ListContainer *last = first;
    ListContainer *c;

    for(c = first; c; c = c->next) {
        last = c;
    }

    for(c = last; c; c = c->prev) {
        VFSNode *n = c->data;
        VFSNode *o = vfs_locate(n, path);

        if(o) {
            VFSInfo i = vfs_query_node(o);

            if(vfs_union_mount_internal(u, NULL, o, i, false)) {
                prim_info = i;
            }
        }
    }

    if(u->vunion.members.primary) {
        if(!u->vunion.members.all->next || !prim_info.is_dir) {
            // the temporary union contains just one member, or doesn't represent a directory
            // in those cases it's just a useless wrapper, so let's just return the primary member directly
            VFSNode *n = u->vunion.members.primary;

            // remove the primary member from the 'all' list to prevent vfs_freetemp from wrecking it
            delete_element((void**)&u->vunion.members.all, u->vunion.members.all);

            vfs_freetemp(u);
            return n;
        }
    } else {
        // all in vain...
        vfs_freetemp(u);
        u = NULL;
    }

    return u;
}

typedef struct VFSUnionIterData {
    Hashtable *visited;
    ListContainer *current;
    void *opaque;
} VFSUnionIterData;

static const char* vfs_union_iter(VFSNode *node, void **opaque) {
    VFSUnionIterData *i = *opaque;
    const char *r = NULL;

    if(!i) {
        i = malloc(sizeof(VFSUnionIterData));
        i->current = node->vunion.members.all;
        i->opaque = NULL;

         // XXX: this may not be the most efficient implementation of a "set" structure...
        i->visited = hashtable_new_stringkeys(37);

        *opaque = i;
    }

    while(i->current) {
        VFSNode *n = (VFSNode*)i->current->data;
        r = vfs_iter(n, &i->opaque);

        if(!r) {
            vfs_iter_stop(n, &i->opaque);
            i->opaque = NULL;
            i->current = i->current->next;
            continue;
        }

        if(hashtable_get_string(i->visited, r)) {
            continue;
        }

        hashtable_set_string(i->visited, r, (void*)true);
        break;
    }

    return r;
}

static void vfs_union_iter_stop(VFSNode *node, void **opaque) {
    VFSUnionIterData *i = *opaque;

    if(i) {
        hashtable_free(i->visited);
        free(i);
    }

    *opaque = NULL;
}

static VFSInfo vfs_union_query(VFSNode *node) {
    if(node->vunion.members.primary) {
        return vfs_query_node(node->vunion.members.primary);
    }

    vfs_set_error("Union object has no members");
    return VFSINFO_ERROR;
}

static bool vfs_union_mount_internal(VFSNode *unode, const char *mountpoint, VFSNode *mountee, VFSInfo info, bool seterror) {
    if(!info.exists) {
        if(seterror) {
            char *r = vfs_repr_node(mountee, true);
            vfs_set_error("Mountee doesn't represent a usable resource: %s", r);
            free(r);
        } else {
            vfs_decref(mountee);
        }

        return false;
    }

    if(seterror && !info.is_dir) {
        char *r = vfs_repr_node(mountee, true);
        vfs_set_error("Mountee is not a directory: %s", r);
        free(r);
        return false;
    }

    ListContainer *c = NULL, *p = unode->vunion.members.all;
    create_container(&c)->data = mountee;

    c->next = p;

    if(p) {
        p->prev = c;
    }

    unode->vunion.members.all = c;
    unode->vunion.members.primary = mountee;

    return true;
}

static bool vfs_union_mount(VFSNode *unode, const char *mountpoint, VFSNode *mountee) {
    if(mountpoint) {
        vfs_set_error("Attempted to use a named mountpoint '%s' on a union", mountpoint);
        return false;
    }

    return vfs_union_mount_internal(unode, NULL, mountee, vfs_query_node(mountee), true);
}

static SDL_RWops* vfs_union_open(VFSNode *unode, VFSOpenMode mode) {
    VFSNode *n = unode->vunion.members.primary;

    if(n) {
        if(n->funcs->open) {
            return n->funcs->open(n, mode);
        } else {
            vfs_set_error("Primary union member can't be opened as a file");
        }
    } else {
        vfs_set_error("Union object has no members");
    }

    return NULL;
}

static char* vfs_union_repr(VFSNode *node) {
    char *mlist = strdup("union: "), *r;

    for(ListContainer *c = node->vunion.members.all; c; c = c->next) {
        VFSNode *n = c->data;

        strappend(&mlist, r = vfs_repr_node(n, false));
        free(r);

        if(c->next) {
            strappend(&mlist, ", ");
        }
    }

    return mlist;
}

static char* vfs_union_syspath(VFSNode *node) {
    VFSNode *n = node->vunion.members.primary;

    if(n) {
        if(n->funcs->syspath) {
            return n->funcs->syspath(n);
        } else {
            vfs_set_error("Primary union member doesn't represent a real filesystem path");
        }
    }

    vfs_set_error("Union object has no members");
    return NULL;
}

static bool vfs_union_mkdir(VFSNode *node, const char *subdir) {
    VFSNode *n = node->vunion.members.primary;

    if(n) {
        if(n->funcs->mkdir) {
            return n->funcs->mkdir(node, subdir);
        } else {
            vfs_set_error("Primary union member doesn't support directory creation");
        }
    }

    vfs_set_error("Union object has no members");
    return false;
}

static VFSNodeFuncs vfs_funcs_union = {
    .repr = vfs_union_repr,
    .query = vfs_union_query,
    .free = vfs_union_free,
    .syspath = vfs_union_syspath,
    .mount = vfs_union_mount,
    .locate = vfs_union_locate,
    .iter = vfs_union_iter,
    .iter_stop = vfs_union_iter_stop,
    .mkdir = vfs_union_mkdir,
    .open = vfs_union_open,
};

void vfs_union_init(VFSNode *node) {
    node->type = VNODE_UNION;
    node->funcs = &vfs_funcs_union;
}
