#include "sputnik.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>

struct stSPUTWindow {
    xcb_connection_t *c;
    xcb_screen_t *s;

    xcb_drawable_t win;
    xcb_gcontext_t gc;
    xcb_pixmap_t pixmap;
};

SPUTWindow *sputCreateWindow(SP_CONST char *title,
                             size_t width,
                             size_t height)
{
    SPUTWindow *window = malloc(sizeof(SPUTWindow));
    if (!window) {
        return NULL;
    }

    xcb_connection_t *c = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(c)) {
        free(window);
        return NULL;
    }

    xcb_screen_t *s = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

    xcb_drawable_t win = xcb_generate_id(c);
    xcb_create_window(c,
                      XCB_COPY_FROM_PARENT,
                      win,
                      s->root,
                      0, 0,
                      width, height,
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      s->root_visual,
                      0,
                      NULL);

    xcb_change_property(c,
                        XCB_PROP_MODE_REPLACE,
                        win,
                        XCB_ATOM_WM_NAME,
                        XCB_ATOM_STRING,
                        8,
                        strlen(title),
                        title);

    xcb_pixmap_t pixmap = xcb_generate_id(c);
    xcb_create_pixmap(c,
                      s->root_depth,
                      pixmap,
                      win,
                      width, height);

    xcb_gcontext_t gc = xcb_generate_id(c);
    xcb_create_gc(c,
                  gc,
                  win,
                  0,
                  NULL);

    xcb_map_window(c, win);
    xcb_flush(c);

    window->c = c;
    window->s = s;
    window->win = win;
    window->gc = gc;
    window->pixmap = pixmap;

    return window;
}

bool sputWindowDisplay(SPUTWindow *window, SoftpipeFramebuffer *fb) {
    size_t fbWidth, fbHeight;
    spGetFramebufferSize(fb, &fbWidth, &fbHeight);

    uint8_t *pixels =
        malloc(fbWidth * fbHeight * 4 * sizeof(uint8_t));
    spReadPixelBGRA32(fb, pixels);

    xcb_connection_t *c = window->c;
    xcb_screen_t *s = window->s;
    xcb_drawable_t win = window->win;
    xcb_gcontext_t gc = window->gc;
    xcb_pixmap_t pixmap = window->pixmap;

    xcb_image_t *image = xcb_image_create_native(c,
                                                 fbWidth,
                                                 fbHeight,
                                                 XCB_IMAGE_FORMAT_Z_PIXMAP,
                                                 s->root_depth,
                                                 pixels,
                                                 fbWidth * fbHeight * 4,
                                                 pixels);

    xcb_image_put(c,
                  pixmap,
                  gc,
                  image,
                  0, 0,
                  0);

    xcb_copy_area(c,
                  pixmap,
                  win,
                  gc,
                  0, 0,
                  0, 0,
                  fbWidth, fbHeight);

    xcb_image_destroy(image);

    xcb_flush(c);
    return true;
}

void sputWindowMainLoop(SPUTWindow *window) {
    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(window->c))) {
        switch (e->response_type & ~0x80) {
        case XCB_EXPOSE: {
            xcb_expose_event_t *ev = (xcb_expose_event_t *)e;
            if (ev->window == window->win) {
                xcb_copy_area(window->c,
                              window->pixmap,
                              window->win,
                              window->gc,
                              0, 0,
                              0, 0,
                              window->s->width_in_pixels,
                              window->s->height_in_pixels);
                xcb_flush(window->c);
            }
            break;
        }
        }

        free(e);
    }
}

void sputDestroyWindow(SPUTWindow *window) {
    xcb_connection_t *c = window->c;

    xcb_free_pixmap(c, window->pixmap);
    xcb_free_gc(c, window->gc);
    xcb_destroy_window(c, window->win);
    xcb_disconnect(c);
    free(window);
}
