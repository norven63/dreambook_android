/**
 * 该文件生成的.so文件需要放置在Android项目中的libs/armeabi-v7a/文件下面才能正常加载
 */
#include <jni.h>
#include <android/bitmap.h>
#include <incEngine.h>
#include "engine/cn_retech_rich_text_reader_BookReaderJni.h"

struct SGlobalEnv {
    jobject crbClickListener;
    jobject calcLayoutListener;
    JavaVM *pJavaVM;

    SGlobalEnv() {
      crbClickListener = NULL;
      calcLayoutListener = NULL;
      pJavaVM = NULL;
    }
};

SGlobalEnv gs;

class MyIProgressReceiver: public IProgressReceiver {
  public:
    JNIEnv *currentEnv;
    jclass cx;

    void init() {
      //通过缓存的全局虚拟机对象获得当前线程的Env对象
      gs.pJavaVM->AttachCurrentThread(&currentEnv, NULL);

      //获得监听对象的class
      cx = currentEnv->GetObjectClass(gs.calcLayoutListener);
    }

    virtual void SetStepProgressRange(int nTotalRange) {
      init();

      jmethodID id_setProgressRange = currentEnv->GetMethodID(cx, "onProgressRangeBack", "(I)V");
      currentEnv->CallVoidMethod(gs.calcLayoutListener, id_setProgressRange, nTotalRange);
    }

    virtual void SetStepProgressPos(int nPos) {
      jmethodID id_update = currentEnv->GetMethodID(cx, "onUpdateProgressPos", "(I)V");
      currentEnv->CallVoidMethod(gs.calcLayoutListener, id_update, nPos);
    }

    virtual void Finish(bool bSuccess) {
      gs.pJavaVM->DetachCurrentThread();
    }
};

MyIProgressReceiver progressReceiver;

class MyCRBClickObserver: public CRBClickObserver {
  public:

};

MyCRBClickObserver gRbClickObserver;

CVideo* OpenVideoFile(LPCTSTR szFilePath, IVideoCB* pCB) {
  return NULL;
}

AGGDC& GetAggDc() {
  static AGGDC inst;
  return inst;
}

#define JniPrefix(x) Java_com_retech_reader_android_richbook_##x
#define methodName(x) Java_cn_retech_rich_1text_1reader_BookReaderJni_##x

extern "C" {
JNIEXPORT void JNICALL JniPrefix(OnInit)
(JNIEnv *, jobject,jstring);
}

JNIEXPORT void JNICALL JniPrefix(OnInit)
(JNIEnv *, jobject,jstring){
  static CRichBook book;
  book.GotoPage(0);
  return;
}

/**
 * 初始化环境
 */
JNIEXPORT void JNICALL methodName(initGlobalSettings)(JNIEnv *env, jclass,jint jwidth, jint jheight, jint jdpi) {
  SetDefaultCharset("UTF-8");

  //初始化资源目录
  CGlobalParam::GetSingleInst().SetParamPath(A2W("/storage/sdcard0/DreamBook/global.param"));
  CGlobalParam::GetSingleInst().Load();

  //设置agg使用的字体目录
  AggSetFontPath(A2W("/storage/sdcard0/DreamBook/font"));

  //设置屏幕分辨率和dpi
  SetScreenParameter((int)jwidth,(int)jheight,(int)jdpi);

  //设置页边距
  GetBookTheme().SetMarginInMM(3,2,3,2);

  //初始化渲染器
  AGGDC& aggDC = GetAggDc();
  aggDC.Resize(jwidth, jheight);

  //设置log路径
  if(WriteDirectory(A2W("/storage/sdcard0/DreamBook/Log"))){
    TRACE("初始化环境成功!");
  }else{
    TRACE("初始化环境失败!");
  }
}

/**
 * 设置各项回调
 */
JNIEXPORT void JNICALL methodName(setListeners)(JNIEnv *env, jclass, jobject crbClickListener, jobject calcLayoutListener) {
  //这里需要注意到是,NewGlobalRef()函数是为了稳固住该回调对象的引用,防止虚拟机将该对象挪动
  gs.crbClickListener = env->NewGlobalRef(crbClickListener);
  gs.calcLayoutListener = env->NewGlobalRef(calcLayoutListener);

  //缓存一个虚拟机对象的指针
  env->GetJavaVM(&gs.pJavaVM) ;
}

/**
 * 判断是否能够渲染当前页
 */
JNIEXPORT jboolean JNICALL methodName(isCurPosVisible)(JNIEnv *, jclass) {
  return GetBookInst().IsCurPosVisible() != 0; //注意这里的Boolean转换!
}

/**
 * 加载书籍
 */
JNIEXPORT jboolean JNICALL methodName(loadBook)(JNIEnv *env, jclass, jstring bookPath) {
  CRichBook& mBook = GetBookInst();

  mBook.SetClickObserver(&gRbClickObserver);
  mBook.SetCalcLayoutObserver(&progressReceiver);

  //根据传入路径,加载书籍
  const char* path = env->GetStringUTFChars(bookPath, NULL);
  mBook.Load(A2W(path) );

  TRACE("书籍加载成功!!!");

  return GetBookInst().IsCurPosVisible() != 0;
}

//渲染书籍
JNIEXPORT void methodName(drawBookContent)(JNIEnv *env, jclass, jintArray jrectValues, jobject bitmap, jboolean isFullScreen) {
  int* rectValues = env->GetIntArrayElements(jrectValues,NULL);

  int left = rectValues[0];
  int top = rectValues[1];
  int right = rectValues[2];
  int bottom = rectValues[3];

  env->ReleaseIntArrayElements(jrectValues,rectValues,0);

  CRichBook& mBook = GetBookInst();

  //将书籍图像布置到AGGDC上
  AGGDC& aggDC = GetAggDc();
  aggDC.SetUpdateRectInDP(CRect(left, top, right, bottom));
  mBook.Render(aggDC);

  //渲染AGGDC至Bitmap上
  void* p1;
  AndroidBitmap_lockPixels(env, bitmap, &p1);

  memcpy(p1, aggDC.GetBuffer(), right * bottom * 4);
//  if(isFullScreen){
//    memcpy(p1, aggDC.GetBuffer(), right * bottom * 4);
//  }else{
//    jclass bitmapClass = env->GetObjectClass(bitmap);
//    jmethodID id_getWidth = env->GetMethodID(bitmapClass, "getWidth", "()I");
//    int bitmapWidth = env->CallIntMethod(bitmap, id_getWidth);
//    jmethodID id_getHeight = env->GetMethodID(bitmapClass, "getHeight", "()I");
//    int bitmapHeight = env->CallIntMethod(bitmap, id_getHeight);
//
//    for(int i = top; i < bottom; i++){
//      int offset=bitmapWidth*i*4+left;
//      memcpy(p1+offset,aggDC.GetBuffer()+offset,(right-left)*4);
//    }
//  }

  AndroidBitmap_unlockPixels(env, bitmap);
}

/**
 * 下一页
 */
JNIEXPORT jboolean JNICALL methodName(nextPage)(JNIEnv *env, jclass) {
  CRichBook& mBook = GetBookInst();
  mBook.GotoNextPage();

  return mBook.IsCurPosVisible() != 0;
}

/**
 * 上一页
 */
JNIEXPORT jboolean JNICALL methodName(lastPage)(JNIEnv *env, jclass) {
  CRichBook& mBook = GetBookInst();
  mBook.GotoLastPage();

  return mBook.IsCurPosVisible() != 0;
}

/**
 * 跳转至指定页
 */
JNIEXPORT jboolean JNICALL methodName(goToPage)(JNIEnv *env, jclass, jint pageNumber) {
  CRichBook& mBook = GetBookInst();
  mBook.GotoPage((int)pageNumber);

  return mBook.IsCurPosVisible() != 0;
}

/**
 * 设置字体大小
 */
JNIEXPORT jboolean JNICALL methodName(setFontSize)(JNIEnv *, jclass, jint fontSize) {
  CRichBook& mBook = GetBookInst();
  mBook.ChangeFontSizeLevel((int)fontSize);

  return mBook.IsCurPosVisible() != 0;
}

/**
 * 选中要拖拽的语句块
 */
JNIEXPORT void JNICALL methodName(selSentence)(JNIEnv *, jclass, jfloat x,jfloat y) {
  GetBookInst().SelSentence(CPoint(x,y));
}

/**
 * 准备拖拽下划线
 */
JNIEXPORT void JNICALL methodName(dragComment)(JNIEnv *, jclass, jfloat x,jfloat y) {
  GetBookInst().OnTouchStart(CPoint(x,y),dragComment);
}

/**
 * 准备拖拽语句块
 */
JNIEXPORT jboolean JNICALL methodName(dragSentence)(JNIEnv *, jclass, jfloat x,jfloat y) {
 return GetBookInst().OnTouchStart(CPoint(x,y)) != 0;
}

/**
 * 点击事件(手指抬起)
 */
JNIEXPORT void JNICALL methodName(onClick)(JNIEnv *, jclass, jfloat x,jfloat y) {
  GetBookInst().Click(CPoint(x,y));
}

/**
 * 手指移动事件
 */
JNIEXPORT void JNICALL methodName(onTouchMove)(JNIEnv *env, jclass, jfloat x,jfloat y, jintArray jrectValues) {
  CRichBook& mBook = GetBookInst();
  mBook.OnTouchMove(CPoint(x,y));

  CRect rc = mBook.GetInvalidRect();
  ScreenLp2Dp(rc);//矩阵单位转换

  int* rectValues = env->GetIntArrayElements(jrectValues,NULL);
  rectValues[0]=rc.left;
  rectValues[1]=rc.top;
  rectValues[2]=rc.right;
  rectValues[3]=rc.bottom;

  env->ReleaseIntArrayElements(jrectValues,rectValues,0);
}

/**
 * 取消选择语句状态
 */
JNIEXPORT void JNICALL methodName(removeBookmark)(JNIEnv *, jclass) {
  GetBookInst().RemoveSel();
}
