/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "cocoa/CCSet.h"
#include "CCDirector.h"
#include "keypad_dispatcher/CCKeypadDispatcher.h"
#include "touch_dispatcher/CCTouch.h"
#include "../CCEGLView.h"
#include "touch_dispatcher/CCTouchDispatcher.h"

#include <android/log.h>
#include <jni.h>

using namespace cocos2d;

extern "C" {
    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesBegin(JNIEnv * env, jobject thiz, jlong id, jfloat x, jfloat y, jint tapcount) {
        cocos2d::CCDirector::sharedDirector()->getOpenGLView()->handleTouchesBegin(1, (intptr_t*)&id, &x, &y, &tapcount);
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesEnd(JNIEnv * env, jobject thiz, jlong id, jfloat x, jfloat y, jint tapcount) {
        cocos2d::CCDirector::sharedDirector()->getOpenGLView()->handleTouchesEnd(1, (intptr_t*)&id, &x, &y, &tapcount);
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesMove(JNIEnv * env, jobject thiz, jlongArray ids, jfloatArray xs, jfloatArray ys, jintArray tapcounts) {
        int size = env->GetArrayLength(ids);
        jlong id[size];
        jfloat x[size];
        jfloat y[size];
		jint tapcount[size];

        env->GetLongArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);
		env->GetIntArrayRegion(tapcounts, 0, size, tapcount);
		
		intptr_t intids[size];
		for (int i = 0; i < size; ++i) {
			intids[i] = id[i];
		}

        cocos2d::CCDirector::sharedDirector()->getOpenGLView()->handleTouchesMove(size, intids, x, y, tapcount);
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesCancel(JNIEnv * env, jobject thiz, jlongArray ids, jfloatArray xs, jfloatArray ys, jintArray tapcounts) {
        int size = env->GetArrayLength(ids);
        jlong id[size];
        jfloat x[size];
        jfloat y[size];
		jint tapcount[size];

        env->GetLongArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);
		env->GetIntArrayRegion(tapcounts, 0, size, tapcount);

		intptr_t intids[size];
		for (int i = 0; i < size; ++i) {
			intids[i] = id[i];
		}

        cocos2d::CCDirector::sharedDirector()->getOpenGLView()->handleTouchesCancel(size, intids, x, y, tapcount);
    }

    #define KEYCODE_BACK 0x04
    #define KEYCODE_MENU 0x52

    JNIEXPORT jboolean JNICALL Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeKeyDown(JNIEnv * env, jobject thiz, jint keyCode) {
        CCDirector* pDirector = CCDirector::sharedDirector();
        switch (keyCode) {
            case KEYCODE_BACK:
                  if (pDirector->getKeypadDispatcher()->dispatchKeypadMSG(kTypeBackClicked))
                    return JNI_TRUE;
                break;
            case KEYCODE_MENU:
                if (pDirector->getKeypadDispatcher()->dispatchKeypadMSG(kTypeMenuClicked))
                    return JNI_TRUE;
                break;
            default:
                return JNI_FALSE;
        }
        return JNI_FALSE;
    }
}
