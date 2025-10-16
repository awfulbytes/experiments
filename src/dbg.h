#define force_distortion          l_osc.distortion.on = 1;

#define land_on_hell_floor()      force_distortion                        \
                                  l_osc.distortion.dante = floor_on_hell; \

#define incr_walk_ctr             ++hell_walk_counter;
#define hell_walk()               if (l_osc.distortion.dante < hell &&        \
                                      go_front == 0 &&                        \
                                      hell_walk_counter == 0xffff) {          \
                                         ++l_osc.distortion.dante;            \
                                         force_distortion                     \
                                         if (l_osc.distortion.dante == hell ) \
                                            ++go_front;                       \
                                         hell_walk_counter = 0; }             \

#define back_walk()               if (go_front == 1                  &&           \
                                      hell_walk_counter == 0xffff) {              \
                                         --l_osc.distortion.dante;                \
                                         force_distortion                         \
                                         if (l_osc.distortion.dante == entrance ) \
                                            --go_front;                           \
                                         hell_walk_counter = 0; }                 \

#define hell_walking()           incr_walk_ctr; \
                                 hell_walk();   \
                                 back_walk();
