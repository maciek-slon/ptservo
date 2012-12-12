#include <stdio.h>

#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"

#include <psmove.h>
#include <psmove_tracker.h>

#include "ptproxy/PTProxy.h"

PTProxy *ptp = new PTProxy("/dev/ttyACM0");

float inrange(float v, float mn, float mx) {
	return (v < mn ? mn : (v > mx ? mx : v) );
}


int main(int arg, char** args) {
    int i = 0;
    int count = psmove_count_connected();
    PSMove* controllers[count];

    printf("### Found %d controllers.\nUsing first one...\n", count);
    if (count == 0) {
        return 1;
    }

    void *frame;
    int result;

    // Opening controller
	printf("Opening controller %d\n", i);
	controllers[i] = psmove_connect_by_id(i);
	assert(controllers[i] != NULL);

    fprintf(stderr, "Trying to init PSMoveTracker...");
    PSMoveTracker* tracker = psmove_tracker_new();
    psmove_tracker_set_mirror(tracker, PSMove_True);
    fprintf(stderr, "OK\n");

	while (1) {
		printf("Calibrating controller %d...", i);
		fflush(stdout);
		result = psmove_tracker_enable(tracker, controllers[i]);

		enum PSMove_Bool auto_update_leds =
			psmove_tracker_get_auto_update_leds(tracker, controllers[i]);
		if (result == Tracker_CALIBRATED) {
			printf("OK, auto_update_leds is %s\n",
					(auto_update_leds == PSMove_True)?"enabled":"disabled");
			break;
		} else {
			printf("ERROR - retrying\n");
		}
	}

    while ((cvWaitKey(1) & 0xFF) != 27) {
        psmove_tracker_update_image(tracker);
        psmove_tracker_update(tracker, NULL);

        frame = psmove_tracker_get_frame(tracker);
        if (frame) {
            //cvShowImage("live camera feed", frame);
        }


		float limit = 70;
		float x, y, r;
		psmove_tracker_get_position(tracker, controllers[i], &x, &y, &r);
		float xx = 0.05*(x-320);
		float yy = 0.05*(y-240);
		xx *= fabs(xx);
		yy *= fabs(yy);

		xx = inrange(xx, -limit, limit);
		yy = inrange(yy, -limit, limit);

		printf("x: %10.2f, y: %10.2f, r: %10.2f\n", xx, yy, r);
		ptp->setMotorSpeed(xx, yy);

		ptp->nextStep();
    }

    ptp->setMotorSpeed(0, 0);
    ptp->nextStep();

    psmove_disconnect(controllers[i]);

    psmove_tracker_free(tracker);
    return 0;
}
