#include <nala.h>
#include <kickstart/ringbuffer.h>

#ifndef __NALA
#include <test/ringbuffer/nala_mocks.h>
#endif

TEST(rb_test1)
{
    struct ks_ringbuffer *rb = NULL;
    int rc;
    
    rc = ks_ringbuffer_init(&rb, 1024);

    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_free(rb);

    ASSERT_EQ(rc, KS_OK);

}


TEST(rb_test_read_write_bytes1)
{
    struct ks_ringbuffer *rb = NULL;
    struct ks_ringbuffer_tail *t = NULL;
    char tmp[16];
    int rc;
    
    rc = ks_ringbuffer_init(&rb, 1024);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_new_tail(rb, &t);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_write(rb, "Hello", 5);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_GT(rb->head_index, 0);
    ASSERT_EQ(t->tail_index, 0);

    rc = ks_ringbuffer_read(rb, t, tmp, 5);
    ASSERT_EQ(rc, KS_OK);

    ASSERT_MEMORY(tmp, "Hello", 5);
    ASSERT_EQ(rb->head_index, 5);
    ASSERT_EQ(t->tail_index, 5);

    rc = ks_ringbuffer_free(rb);
    ASSERT_EQ(rc, KS_OK);
}

TEST(rb_test_write_overlap)
{
    struct ks_ringbuffer *rb = NULL;
    struct ks_ringbuffer_tail *t = NULL;
    char tmp[16];
    int rc;
    
    rc = ks_ringbuffer_init(&rb, 8);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_new_tail(rb, &t);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_write(rb, "Hello", 5);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_GT(rb->head_index, 0);
    ASSERT_EQ(t->tail_index, 0);

    rc = ks_ringbuffer_read(rb, t, tmp, 5);
    ASSERT_EQ(rc, KS_OK);

    ASSERT_MEMORY(tmp, "Hello", 5);
    ASSERT_EQ(rb->head_index, 5);
    ASSERT_EQ(t->tail_index, 5);
    memset(tmp, 0, sizeof(tmp));

    rc = ks_ringbuffer_write(rb, "Hello", 5);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_EQ(rb->head_index, 2);
    ASSERT_EQ(t->tail_index, 5);

    rc = ks_ringbuffer_read(rb, t, tmp, 5);
    ASSERT_EQ(rc, KS_OK);

    ASSERT_MEMORY(tmp, "Hello", 5);
    ASSERT_EQ(rb->head_index, 2);
    ASSERT_EQ(t->tail_index, 2);

    rc = ks_ringbuffer_free(rb);
    ASSERT_EQ(rc, KS_OK);

}


TEST(rb_test_write_overlap2)
{
    struct ks_ringbuffer *rb = NULL;
    struct ks_ringbuffer_tail *t = NULL;
    char tmp[16];
    int rc;
    
    rc = ks_ringbuffer_init(&rb, 8);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_new_tail(rb, &t);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_write(rb, "Hello", 5);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_EQ(rb->head_index, 5);
    ASSERT_EQ(t->tail_index, 0);

    memset(tmp, 0, sizeof(tmp));

    rc = ks_ringbuffer_write(rb, "Hello", 5);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_EQ(rb->head_index, 2);

    ASSERT_MEMORY(rb->bfr, "lolloHel", 8);
    ASSERT_EQ(t->tail_index, 2);
    ASSERT_EQ(t->truncated_bytes, 2);

    rc = ks_ringbuffer_read(rb, t, tmp, 8);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_MEMORY(tmp, "lloHello", 8);
    ASSERT_EQ(rb->head_index, 2);
    ASSERT_EQ(t->tail_index, 2);

    rc = ks_ringbuffer_free(rb);
    ASSERT_EQ(rc, KS_OK);

}


TEST(rb_test_write_overlap3)
{
    struct ks_ringbuffer *rb = NULL;
    struct ks_ringbuffer_tail *t = NULL;
    struct ks_ringbuffer_tail *t2 = NULL;
    char tmp[16];
    int rc;
    
    rc = ks_ringbuffer_init(&rb, 8);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_new_tail(rb, &t);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_new_tail(rb, &t2);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_write(rb, "Hello", 5);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_EQ(rb->head_index, 5);
    ASSERT_EQ(t->tail_index, 0);

    memset(tmp, 0, sizeof(tmp));
    
    ASSERT_EQ(t->available, 5);
    ASSERT_EQ(t2->available, 5);
    rc = ks_ringbuffer_read(rb, t2, tmp, 5);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_MEMORY(tmp, "Hello", 5);
    ASSERT_EQ(t2->available, 0);

    rc = ks_ringbuffer_write(rb, "Hello", 5);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_EQ(rb->head_index, 2);

    ASSERT_MEMORY(rb->bfr, "lolloHel", 8);
    ASSERT_EQ(t->tail_index, 2);
    ASSERT_EQ(t->truncated_bytes, 2);

    rc = ks_ringbuffer_read(rb, t, tmp, 8);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_MEMORY(tmp, "lloHello", 8);
    ASSERT_EQ(rb->head_index, 2);
    ASSERT_EQ(t->tail_index, 2);

    rc = ks_ringbuffer_read(rb, t2, tmp, 5);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_MEMORY(tmp, "Hello", 5);

    rc = ks_ringbuffer_free(rb);
    ASSERT_EQ(rc, KS_OK);

}


TEST(rb_test_write_too_much)
{
    struct ks_ringbuffer *rb = NULL;
    int rc;

    rc = ks_ringbuffer_init(&rb, 8);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_write(rb, "01234567", 8);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_EQ(rb->head_index, 0);

    rc = ks_ringbuffer_write(rb, "012345670", 9);
    ASSERT_EQ(rc, KS_ERR);

    rc = ks_ringbuffer_free(rb);
    ASSERT_EQ(rc, KS_OK);
}


TEST(rb_test_read_too_much)
{
    struct ks_ringbuffer *rb = NULL;
    struct ks_ringbuffer_tail *t = NULL;
    char tmp[16];
    int rc;

    rc = ks_ringbuffer_init(&rb, 8);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_new_tail(rb, &t);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_write(rb, "01234567", 8);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_EQ(rb->head_index, 0);

    rc = ks_ringbuffer_read(rb, t, tmp, 8);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_EQ(t->tail_index, 0);
    ASSERT_EQ(rb->head_index, 0);
    ASSERT_MEMORY(tmp, "01234567", 8);

    rc = ks_ringbuffer_read(rb, t, tmp, 1);
    ASSERT_EQ(rc, KS_ERR);

    rc = ks_ringbuffer_read(rb, t, tmp, 32);
    ASSERT_EQ(rc, KS_ERR);

    rc = ks_ringbuffer_write(rb, "01234567", 8);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_read(rb, t, tmp, 8);
    ASSERT_EQ(rc, KS_OK);
    ASSERT_MEMORY(tmp, "01234567", 8);

    rc = ks_ringbuffer_free(rb);
    ASSERT_EQ(rc, KS_OK);
}

uint64_t advance_tail(struct ks_ringbuffer *rb, struct ks_ringbuffer_tail *t,
                      uint64_t new_index)
{
    uint32_t *payload_sz = NULL; 
    uint64_t current_index = t->tail_index;

    while (current_index < new_index)
    {
        payload_sz = (uint32_t *) &rb->bfr[current_index];
        current_index = (current_index+sizeof(uint32_t)+*payload_sz)%rb->bfr_sz;
    }
    
    return current_index;
}

TEST(rb_write_objs)
{
    int rc;
    struct ks_ringbuffer *rb;
    struct ks_ringbuffer_tail *t;
    char tmp[16];

    rc = ks_ringbuffer_init(&rb, 16);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_new_tail(rb, &t);
    ASSERT_EQ(rc, KS_OK);
    
    rc = ks_ringbuffer_set_tail_advance_cb(t, advance_tail);
    ASSERT_EQ(rc, KS_OK);


    /* 0123456789ABCDEF 16 byte buffer*/
    /* t0               Tail at pos 0 */
    /* (Obj1 )(Obj2 )   Two objects written in buffer */
    /* bj3 )  (Obj2 )(O Third object is written  */
    /*        t0        Since there is not room for more than
     *                  Two objects in the buffer
     *                  Object 1 must be removed and tail t0 advanced
     *                  to point at the beginning of object 2  */

    uint32_t payload_length = 3;
    const char *payload = "Hej";
    const char *payload2 = "He2";
    const char *payload3 = "He3";

    /* Write three objects */
    rc = ks_ringbuffer_write(rb, (const char *) &payload_length, 4);
    ASSERT_EQ(rc, KS_OK); 
    rc = ks_ringbuffer_write(rb, payload, 3);
    ASSERT_EQ(rc, KS_OK); 

    rc = ks_ringbuffer_write(rb, (const char *) &payload_length, 4);
    ASSERT_EQ(rc, KS_OK); 
    rc = ks_ringbuffer_write(rb, payload2, 3);
    ASSERT_EQ(rc, KS_OK); 


    ASSERT_EQ(t->tail_index, 0);
    ASSERT_EQ(t->truncated_bytes, 0);

    rc = ks_ringbuffer_write(rb, (const char *) &payload_length, 4);
    ASSERT_EQ(rc, KS_OK); 
    rc = ks_ringbuffer_write(rb, payload3, 3);
    ASSERT_EQ(rc, KS_OK); 

    ASSERT_EQ(t->tail_index, 7);
    ASSERT_EQ(t->truncated_bytes, 7);

    rc = ks_ringbuffer_read(rb, t, tmp, 7);
    ASSERT_EQ(rc, KS_OK);
    uint32_t *readback_sz = (uint32_t *) tmp;
    ASSERT_EQ(*readback_sz, 3);
    ASSERT_MEMORY(&tmp[4], "He2", 3);
    ASSERT_EQ(rb->head_index, 5);
    ASSERT_EQ(t->tail_index, 14);

    rc = ks_ringbuffer_read(rb, t, tmp, 7);
    ASSERT_EQ(rc, KS_OK);
    readback_sz = (uint32_t *) tmp;
    ASSERT_EQ(*readback_sz, 3);
    ASSERT_MEMORY(&tmp[4], "He3", 3);

    ASSERT_EQ(rb->head_index, 5);
    ASSERT_EQ(t->tail_index, 5);

    rc = ks_ringbuffer_free(rb);
    ASSERT_EQ(rc, KS_OK);
}



TEST(rb_remove_tail)
{
    int rc;
    struct ks_ringbuffer *rb;
    struct ks_ringbuffer_tail *t;
    char tmp[16];

    rc = ks_ringbuffer_init(&rb, 16);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_new_tail(rb, &t);
    ASSERT_EQ(rc, KS_OK);
    
    rc = ks_ringbuffer_set_tail_advance_cb(t, advance_tail);
    ASSERT_EQ(rc, KS_OK);


    /* 0123456789ABCDEF 16 byte buffer*/
    /* t0               Tail at pos 0 */
    /* (Obj1 )(Obj2 )   Two objects written in buffer */
    /* bj3 )  (Obj2 )(O Third object is written  */
    /*        t0        Since there is not room for more than
     *                  Two objects in the buffer
     *                  Object 1 must be removed and tail t0 advanced
     *                  to point at the beginning of object 2  */

    uint32_t payload_length = 3;
    const char *payload = "Hej";
    const char *payload2 = "He2";
    const char *payload3 = "He3";

    /* Write three objects */
    rc = ks_ringbuffer_write(rb, (const char *) &payload_length, 4);
    ASSERT_EQ(rc, KS_OK); 
    rc = ks_ringbuffer_write(rb, payload, 3);
    ASSERT_EQ(rc, KS_OK); 

    rc = ks_ringbuffer_write(rb, (const char *) &payload_length, 4);
    ASSERT_EQ(rc, KS_OK); 
    rc = ks_ringbuffer_write(rb, payload2, 3);
    ASSERT_EQ(rc, KS_OK); 


    ASSERT_EQ(t->tail_index, 0);
    ASSERT_EQ(t->truncated_bytes, 0);

    rc = ks_ringbuffer_write(rb, (const char *) &payload_length, 4);
    ASSERT_EQ(rc, KS_OK); 
    rc = ks_ringbuffer_write(rb, payload3, 3);
    ASSERT_EQ(rc, KS_OK); 

    ASSERT_EQ(t->tail_index, 7);
    ASSERT_EQ(t->truncated_bytes, 7);

    rc = ks_ringbuffer_read(rb, t, tmp, 7);
    ASSERT_EQ(rc, KS_OK);
    uint32_t *readback_sz = (uint32_t *) tmp;
    ASSERT_EQ(*readback_sz, 3);
    ASSERT_MEMORY(&tmp[4], "He2", 3);
    ASSERT_EQ(rb->head_index, 5);
    ASSERT_EQ(t->tail_index, 14);

    rc = ks_ringbuffer_read(rb, t, tmp, 7);
    ASSERT_EQ(rc, KS_OK);
    readback_sz = (uint32_t *) tmp;
    ASSERT_EQ(*readback_sz, 3);
    ASSERT_MEMORY(&tmp[4], "He3", 3);

    ASSERT_EQ(rb->head_index, 5);
    ASSERT_EQ(t->tail_index, 5);

    rc = ks_ringbuffer_remove_tail(t);
    ASSERT_EQ(rc, KS_OK);

    rc = ks_ringbuffer_free(rb);
    ASSERT_EQ(rc, KS_OK);
}

TEST(rb_nullargs)
{
    int p;
    int rc;

    rc = ks_ringbuffer_init(NULL, 0);
    ASSERT_EQ(rc, KS_ERR);

    rc = ks_ringbuffer_init((struct ks_ringbuffer **) &p, 0);
    ASSERT_EQ(rc, KS_ERR);

    struct ks_ringbuffer *rb;
    ks_ll_init_mock_once(KS_ERR);
    rc = ks_ringbuffer_init(&rb, 123);
    ASSERT_EQ(rc, KS_ERR);

    rc = ks_ringbuffer_free(NULL);
    ASSERT_EQ(rc, KS_ERR);

    struct ks_ringbuffer rb2;
    ks_ll_free_mock_once(KS_ERR);
    rc = ks_ringbuffer_free(&rb2);
    ASSERT_EQ(rc, KS_ERR);

    rc = ks_ringbuffer_new_tail(NULL, NULL);
    ASSERT_EQ(rc, KS_ERR);

    rc = ks_ringbuffer_new_tail(rb, NULL);
    ASSERT_EQ(rc, KS_ERR);

    rc = ks_ringbuffer_remove_tail(NULL);
    ASSERT_EQ(rc, KS_ERR);
    

}

TEST(rb_null_set_tail_advance)
{
    int rc;

    rc = ks_ringbuffer_set_tail_advance_cb(NULL, NULL);
    ASSERT_EQ(rc, KS_ERR);
    struct ks_ringbuffer_tail t;

    rc = ks_ringbuffer_set_tail_advance_cb(&t, NULL);
    ASSERT_EQ(rc, KS_ERR);
}

TEST(rb_write_null)
{
    int rc;
    rc = ks_ringbuffer_write(NULL, NULL, 0);
    ASSERT_EQ(rc, KS_ERR);
    struct ks_ringbuffer r;
    rc = ks_ringbuffer_write(&r, NULL, 0);
    ASSERT_EQ(rc, KS_ERR);
    rc = ks_ringbuffer_read(NULL, NULL, NULL, 0);
    ASSERT_EQ(rc, KS_ERR);
    
    rc = ks_ringbuffer_read(&r, NULL, NULL, 0);
    ASSERT_EQ(rc, KS_ERR);

    struct ks_ringbuffer_tail t;
    r.bfr_sz = 999999;
    t.available = 99999;
    r.head_index = 1;
    t.tail_index = 0;
    rc = ks_ringbuffer_read(&r, &t, NULL, 9999);
    ASSERT_EQ(rc, KS_ERR);


    r.bfr_sz = 9;
    t.available = 99999;
    r.head_index = 0;
    t.tail_index = 1;
    rc = ks_ringbuffer_read(&r, &t, NULL, 9999);
    ASSERT_EQ(rc, KS_ERR);
}

TEST(rb_new_tail)
{
    int rc;
    struct ks_ringbuffer *rb2 = malloc(sizeof(struct ks_ringbuffer));
    struct ks_ringbuffer_tail *tail;
    ks_ll_append2_mock_once(40, KS_ERR);
    rc = ks_ringbuffer_new_tail(rb2, &tail);
    ASSERT_EQ(rc, KS_ERR);
    free(rb2);
}
