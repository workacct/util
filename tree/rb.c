#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef enum rbc_ {
    blk = 0,
    red = 1,
} rbc;

typedef struct rbnode_ {
    int d_;
    int c_;
    struct rbnode_ *l_;
    struct rbnode_ *r_;
    struct rbnode_ *p_;
} rbnode;

typedef rbnode *rbpos;
typedef rbpos rbtree;

//      X            Y
//     / \          / \       
//    Y   a        Z   X
//   / \          /   / \
//  Z   b        c   b   a
rbtree srl(rbtree t, rbpos n)
{
    rbpos r = n->r_, p = n->p_;
    n->r_ = r->l_;
    r->l_ = n;
    r->p_ = p;
    if (NULL == p) t = r;
    else if (p->d_ > n->d_) p->l_ = r;
    else p->r_ = r; 
    if (n->r_) n->r_->p_ = n;
    return t;
}

rbtree srr(rbtree t, rbpos n)
{
    rbpos l = n->l_, p = n->p_;
    n->l_ = l->r_;
    l->r_ = n;
    l->p_ = p;
    if (NULL == p) t = l;
    else if (p->d_ > n->d_) p->l_ = l;
    else p->r_ = l;
    if (n->l_) n->l_->p_ = n;
    return t;
}

rbtree rbtree_insert_rebalance(rbtree t, rbpos node)
{
    rbpos uncle = NULL, tmp = NULL, parent = node->p_;
    while (parent && red == parent->c_) {
        rbpos gp = parent->p_;
        if (parent == gp->l_) {
            uncle = gp->r_;
            if (red == uncle->c_) {
                uncle->c_ = blk;
                parent->c_ = blk;
                gp->c_ = red;
                node = gp;
                parent = node->p_;
                continue;
            }
            if (node == parent->r_) {
                t = srl(t, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }
            gp->c_ = red;
            parent->c_ = blk;
            t = srr(t, gp);
            node = parent;
            parent = node->p_;
        } else {
            uncle = gp->l_;
            if (red == uncle->c_) {
                uncle->c_ = blk;
                parent->c_ = blk;
                gp->c_ = red;
                node = gp;
                parent = node->p_;
                continue;
            }
            if (node == parent->l_) {
                t = srr(t, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }
            gp->c_ = red;
            parent->c_ = blk;
            t = srl(t, gp);
            node = parent;
            parent = node->p_;
        }
    }
    if (t) t->c_ = blk;
    return t;
}

rbtree rbtree_insert(rbtree t, int ele)
{
    rbpos r = t, p = NULL;
    while (t && ele != t->d_) {
        p = t;
        if (ele < t->d_) t = t->l_;
        else t = t->r_; 
    }
    if (t) {
        printf("%d is exist\n",ele);
        return r;
    }
    t = (rbpos)calloc(1, sizeof(rbnode));
    if (NULL == t) {
        printf("insert OOM!");
        return r;
    }
    t->r_ = t->l_ = t->p_ = NULL;
    t->d_ = ele;
    t->c_ = red;
    
    if (NULL == p) {
        t->c_ = blk;
        return t;
    }
    if (p->d_ > ele) p->l_ = t;
    else p->r_ = t;
    if (red == p->c_) r = rbtree_insert_rebalance(r, t);
    return r;
} 

rbtree rbtree_delete_rebalance(rbtree t, rbpos parent, rbpos node)
{
     rbpos bro = NULL;
     while ((NULL == node||blk == node->c_) && t == node) {
         if (node == parent->l_) {
             bro = parent->r_;
             if (red == bro->c_) {
                 bro->c_ = blk;
                 parent->c_ = red;
                 t = srl(t, parent);
                 bro = parent->r_;
             } else {
                 if ((NULL == bro->l_ || blk == bro->l_->c_) &&
                     (NULL == bro->r_ || blk == bro->r_->c_)) {
                     bro->c_ = red;
                     node = parent;
                     parent = node->p_;
                 } else {
                     if (NULL == bro->r_ || blk == bro->r_->c_) {
                         bro->c_ = red;
                         if (bro->l_) bro->l_->c_ = blk;
                         t = srr(t, bro);
                         bro = parent->r_;    
                     } 
                     bro->c_ = parent->c_;
                     parent->c_ = blk;
                     if (bro->r_) bro->r_->c_ = blk;
                     t = srl(t, parent);
                     node = t;
                 }
             }
         } else {
             bro = parent->l_;
             if (red == bro->c_) {
                 bro->c_ = blk;
                 parent->c_ = red;
                 t = srr(t, parent);
                 bro = parent->l_;
             } else {
                 if ((NULL == bro->l_ || blk == bro->l_->c_) &&
                     (NULL == bro->r_ || blk == bro->r_->c_)) {
                     bro->c_ = red;
                     node = parent;
                     parent = node->p_;
                 } else {
                     if (NULL == bro->l_ || blk == bro->l_->c_) {
                         bro->c_ = red;
                         if (bro->r_) bro->r_->c_ = blk;
                         t = srl(t, bro);
                         bro = parent->l_;    
                     } 
                     bro->c_ = parent->c_;
                     parent->c_ = blk;
                     if (bro->l_) bro->l_->c_ = blk;
                     t = srr(t, parent);
                     node = t;
                 }
             }
         }
     }
     if (t) t->c_ = blk;
     return t;
}

rbtree rbtree_delete(rbtree t, int ele)
{
    rbpos r = t, p = NULL, child = NULL;
    rbc color = blk;
    while (t && ele != t->d_) {
        p = t;
        if (ele < t->d_) t = t->l_;
        else t = t->r_;
    }
    if (NULL == t) {
        printf("%d is not exist\n", ele);
        return r;
    }
    if (t->l_ && t->r_) {
        rbpos tmp = t;
        p = t;
        t = t->l_;
        while (t && t->r_) {p=t;t=t->r_;}
        if (p == tmp) p->l_ = t->l_;
        else p->r_ = t->l_;
        if (t->l_) t->l_->p_ = p; 
        tmp->d_ = t->d_;
        child = t->l_;
    } else if (NULL == t->l_) {
        if (NULL == p) r = t->r_;
        else if (p->d_ > ele) p->l_ = t->r_;
        else p->r_ = t->r_;
        child = t->r_;
    } else {
        if (NULL == p) r = t->l_;
        else if (p->d_ > ele) p->l_ = t->l_;
        else p->r_ = t->l_;
        child = t->l_;
    }
    color = t->c_;
    if (t) free(t);
    if (blk == color) r = rbtree(r, t, child);
    return r;
}
