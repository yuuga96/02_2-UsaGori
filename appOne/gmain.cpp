#if 1//←0を1に変えて作り始めよう！


/*---------------------------------------------------------
制作手順
⓪全体の骨組み、構造体型のデータ内容を読み取っておく
  gmain()の中身。drawはplayとresultそれぞれで呼び出すようにする。
  title()でしていることは何か。
  画像ファイルはどこにあるのか。
  ファイル名は？ 画像サイズは？ 下の動画を参考にしよう。
  https://youtu.be/ZExz4iMFXfE ←ctrlを押しながらここをクリックで再生
①ウサギをrectMode(CENTER)で表示してから、
  キー操作でウィンドウの外に出ないようにする
②SPACEキーが押された瞬間、ウサギがニンジンを発射。
　その後、うんこを移動、描画する。
  https://youtu.be/l6tC2mFwefI 
③ゴリラを左右に動かす
  参考動画はjavascriptだけど、アルゴリズムは同じ
  https://youtu.be/pwEkpPuhtRU
  (sin関数を使ってpxを求めれば、滑らかに動かすことも可能)
④うんこのHPが0の時、ゴリラにうんこさせる
⑤当たり判定～やられ画像への切り替え。下の動画を参考に作る。
　ただし、今回 px,pyは画像の中心座標であることに注意！
  halfW,halfHをうまく使おう。
  https://youtu.be/IT-_DmXivjQ
⑥HPバーの表示
  rectを使おう。strokeWeight(0)にして輪郭表示をしない。
⑦勝敗判定し、resultへ。リプレイの処理。
⑧タイトル画面の表示。
---------------------------------------------------------*/
#include "libOne.h"
#include "gmain.h"

//ウサギとゴリラ用構造体型
struct CHARA {
    //各画像番号
    int normalImg;
    int damageImg;
    int loseImg;
    int winImg;
    //現在の画像番号
    int img;
    //位置
    float px;
    float py;
    //移動方向ベクトル
    float vx;
    float vy;
    //弾発射オフセット位置
    float bulletOfsY;//pyにこの値を加算した値を発射位置とする
    //体力
    int hp;
    //ＨＰゲージ用画像番号
    int hpGaugeImg;
    //ＨＰゲージ表示オフセット位置
    float hpGaugeOfsY;
    //当たり判定用、半分の幅と高さ
    float halfW;
    float halfH;
};
//にんじんとうんこ用構造体型
struct BULLET {
    //画像番号
    int img;
    //位置
    float px;
    float py;
    //移動方向ベクトル
    float vx;
    float vy;
    //体力？（ウィンドウ内を飛んでいるかを意味する）
    int hp;
    //当たり判定用、半分の幅と高さ
    float halfW;
    float halfH;
};
//全データを管理する構造体型
struct DATA {
    //----------------------------------------
    //ステート制御用データ
    int TITLE = 0;
    int PLAY = 1;
    int RESULT = 2;
    int state = TITLE;
    //RESULTの時、キー入力を無効化するフレーム数
    int frameRestrictInput;
    int frameCnt;
    //----------------------------------------
    //キャラクタデータ
    struct CHARA rabit;
    struct CHARA gori;
    struct BULLET ninjin;
    struct BULLET unko;
    int titleImg;
    int backImg;
};

//関数の呼び出し構造
//・loading()
//・title()
//    init()
//・play()
//    collision()
//    draw()
//      hpGauge()
//・result()
//    draw()
//      hpGauge();
void loading(struct DATA* d) {
    d->rabit.normalImg = loadImage("assets\\rabit0.png");
    d->rabit.damageImg = loadImage("assets\\rabit1.png");
    d->rabit.loseImg = loadImage("assets\\rabit2.png");
    d->rabit.winImg = loadImage("assets\\rabit3.png");

    d->gori.normalImg = loadImage("assets\\gori0.png");
    d->gori.damageImg = loadImage("assets\\gori1.png");
    d->gori.loseImg = loadImage("assets\\gori2.png");
    d->gori.winImg = loadImage("assets\\gori3.png");

    d->unko.img = loadImage("assets\\unko0.png");
    d->ninjin.img = loadImage("assets\\ninjin0.png");

    d->titleImg = loadImage("assets\\title.png");
    d->backImg = loadImage("assets\\back.png");
}
void title(struct DATA* d){
    //仮の処理！ すぐにinitを呼び出してPLAYへ
    clear();
    rectMode(CENTER);
    imageColor(255, 255, 255);
    image(d->titleImg, 0, 0);
    if (isTrigger(KEY_SPACE)) {
        init(d);
        d->state = d->PLAY;
    }
}
void init(struct DATA* d){
    d->rabit.img = d->rabit.normalImg;
    d->rabit.px = 150.0f;
    d->rabit.py = 400.0f;
    d->rabit.vx = 5.0f;
    d->rabit.hp = 50;
    d->rabit.bulletOfsY = -70;
    d->rabit.hpGaugeOfsY = -60;

    d->gori.img = d->gori.normalImg;
    d->gori.px = 150.0f;
    d->gori.py = 70.0f;
    d->gori.vx = 5.0f;
    d->gori.hp = 100;
    d->gori.bulletOfsY = 55;
    d->gori.hpGaugeOfsY = -62;

    d->unko.px = 0.0f;
    d->unko.py = 0.0f;
    d->unko.vy = 5.0f;
    d->unko.hp = 0;

    d->ninjin.px = 0.0f;
    d->ninjin.py = 0.0f;
    d->ninjin.vy = -5.0f;
    d->ninjin.hp = 0;

    //当たり判定用データ
    d->rabit.halfW = 35;
    d->rabit.halfH = 55;
    d->gori.halfW = 40;
    d->gori.halfH = 60;
    d->unko.halfW = 20;
    d->unko.halfH = 16;
    d->ninjin.halfW = 10;
    d->ninjin.halfH = 16;

    d->frameRestrictInput = 90;



}
void play(struct DATA* d){
    //ウサギを動かす
    if (isPress(KEY_A)) {d->rabit.px += -d->rabit.vx;}
    if (isPress(KEY_D)) { d->rabit.px += d->rabit.vx; }
    //ウサギの移動範囲制限
    if (d->rabit.px > width - d->rabit.halfW) {
        d->rabit.px = width - d->rabit.halfW;
    }
    //ニンジンを発射する
    if (d->ninjin.hp == 0 && isTrigger(KEY_SPACE)) {
        //発射位置
        d->ninjin.py < -d->rabit.px;
        d->ninjin.py = d->rabit.py + d->rabit.bulletOfsY;
        d->ninjin.hp = 1;
    }
        //ニンジンを動かす
    if (d->ninjin.hp > 0) {
        d->ninjin.py += d->ninjin.vy;
        //ウィンドウの外に出た
        if (d->ninjin.py < -d->ninjin.halfH) {
            d->ninjin.hp = 0;
        }
    }
    //ゴリラの動き
    d->gori.px += d->gori.vx;
    if (d->gori.px < d->gori.halfW ||
        d->gori.px > width - d->gori.halfW) {
        d->gori.vx = -d->gori.vx;
    }
    //ウンコの動き
    if (d->unko.hp == 0 ) {
        //発射位置
        d->unko.py < -d->gori.px;
        d->unko.py = d->gori.py + d->gori.bulletOfsY;
        d->unko.hp = 1;
    }
    //ウンコを動かす
    if (d->unko.hp > 0) {
        d->unko.py += d->unko.vy;
        //ウィンドウの外に出た
        if (d->unko.py > height + d->unko.halfH) {
            d->unko.hp = 0;
        }
    }
    //当たり判定・ウサギ、ウンコ 
    if (collision(&d->rabit, &d->unko)) {
        d->rabit.img = d->rabit.damageImg;
        d->rabit.hp--;
    }
    else {
        d->rabit.img = d->rabit.normalImg;
    }
    //当たり判定ゴリラ、ニンジン
    if (collision(&d->gori, &d->ninjin)) {
        d->gori.img = d->gori.damageImg;
        d->gori.hp--;
    }
    else {
        d->gori.img = d->gori.normalImg;
    }

    //描画
    draw(d);

    //勝負がついた
    if (d->rabit.hp <= 0 || d->gori.hp <= 0) {
        //勝ち負け画像の切り替え
        if (d->gori.hp > 0) {     //ごりHP０以上(ゴリラ勝ち)
            d->gori.img = d->gori.winImg;   //ごりwin
            d->rabit.img = d->rabit.loseImg;    //ウサギlose
        }
        else {       //ウサギの勝ち(ごりが勝ちじゃないなら)
            d->rabit.img = d->rabit.winImg; //ウサギwin
            d->gori.img = d->gori.loseImg;  //ごりlose
        }
        //ステート切り替え
        d->frameCnt = d->frameRestrictInput;
        d->state = d->RESULT;
    }


}
int collision(struct CHARA* c, struct BULLET* b) {
    //当たり判定
    if (b->hp > 0) {
        float cRight = c->px + c->halfW;
        float cLeft = c->px - c->halfW;
        float cTop = c->py - c->halfH;
        float cBottom = c->py + c->halfH;
        float bRight = b->px + b->halfW;
        float bLeft = b->px - b->halfW;
        float bTop = b->py - b->halfH;
        float bBottom = b->py + b->halfH;
        if (cRight < bLeft || bRight < cLeft ||
            bBottom < cTop || cBottom < bTop) {
            //触れていない
            return 0;
        }
        else {
            //触れている
            return 1;
        }
        return 0;
    }
}
void draw(struct DATA* d){
    clear();
    //背景
    rectMode(CENTER);
    image(d->backImg, 0, 0);
    //キャラ
    rectMode(CENTER);
    image(d->rabit.img, d->rabit.px, d->rabit.py);
    image(d->gori.img, d->gori.px, d->gori.py);
    //弾
    if (d->ninjin.hp > 0) {
        image(d->ninjin.img, d->ninjin.px, d->ninjin.py);
    }
    if (d->unko.hp > 0) {

    }
    //ＨＰゲージ
    hpGauge(&d->gori);
    hpGauge(&d->rabit);
#if _DEBUG
    //当たり判定エリア表示
    fill(255, 255, 255, 128);
    rect(d->rabit.px, d->rabit.py, d->rabit.halfW * 2, d->rabit.halfH * 2);
    rect(d->unko.px, d->unko.py, d->unko.halfW * 2, d->unko.halfH * 2);
    rect(d->gori.px, d->gori.py, d->gori.halfW * 2, d->gori.halfH * 2);
    rect(d->ninjin.px, d->ninjin.py, d->ninjin.halfW * 2, d->ninjin.halfH * 2);
#endif
}

void hpGauge(struct CHARA* c){
    strokeWeight(0);
    if (c->hp > 15) {
        fill(0, 255, 0);
    }
    else {
        fill(255, 0, 0);
    }
    rect(c->px, c->py + c->hpGaugeOfsY, c->hp, 5);
}
void result(struct DATA* d){
    //ニンジンとウンコを画面外まで移動
    if (d->ninjin.hp > 0) {
        d->ninjin.py += d->ninjin.vy;
        if (d->ninjin.py < -d->ninjin.halfH) {
            d->ninjin.hp = 0;
        }
    }
    if (d->unko.hp > 0) {
        d->unko.py += d->unko.vy;
        if (d->unko.py > height + d->unko.halfH) {
            d->unko.hp = 0;
        }
    }
    //描画
    draw(d);

    //リスタート(数フレームループしないとスペースキーを押せない)
    if (d->frameCnt > 0) {
        d->frameCnt--;
    }
    else {
        text("Spaceでタイトルに戻ります", 25, height);
        if (isTrigger(KEY_SPACE)) {
            d->state = d->TITLE;
        }
    }
}

void gmain(){
    window(300, 480);
    //全データ
    struct DATA d;
    //全画像読み込み
    loading(&d);
    while (notQuit) {
        //ゲームステート制御
        if      (d.state == d.TITLE ) { title(&d); }
        else if (d.state == d.PLAY  ) { play(&d); }
        else if (d.state == d.RESULT) { result(&d); }
    }
    ShowCursor(true);
}








#else
//---------------------------------------------------------
//ここから完成版
//---------------------------------------------------------
#include "libOne.h"
#include "gmain.h"

//ウサギとゴリラ用構造体型
struct CHARA {
    //各画像番号
    int normalImg;
    int damageImg;
    int loseImg;
    int winImg;
    //現在の画像番号
    int img;
    //位置
    float px;
    float py;
    //移動方向ベクトル
    float vx;
    float vy;
    //弾発射オフセット位置
    float bulletOfsY;
    //体力
    int hp;
    //ＨＰゲージ表示オフセット位置
    float hpGaugeOfsY;
    //当たり判定用、半分の幅と高さ
    float halfW;
    float halfH;
};
//にんじんとうんこ用構造体型
struct BULLET {
    //画像番号
    int img;
    //位置
    float px;
    float py;
    //方向ベクトル
    float vx;
    float vy;
    //体力？（ウィンドウ内を飛んでいるかを意味する）
    int hp;
    //当たり判定用、半分の幅と高さ
    float halfW;
    float halfH;
};
//全データを管理する構造体型
struct DATA {
    //----------------------------------------
    //ステート制御用データ
    int TITLE = 0;
    int PLAY = 1;
    int RESULT = 2;
    int state = TITLE;
    //RESULTの時、キー入力を無効化するフレーム数
    int frameRestrictInput;
    int frameCnt;
    //----------------------------------------
    //キャラクタデータ
    struct CHARA rabit;
    struct CHARA gori;
    struct BULLET ninjin;
    struct BULLET unko;
    int titleImg;
    int backImg;
};

//関数の呼び出し構造
//title()
//  init()
//play()
//  collision()
//  draw()
//    hpGauge()
//result()
//  draw()
//    hpGauge();
void loading(struct DATA* d) {
    d->rabit.normalImg = loadImage("assets\\rabit0.png");
    d->rabit.damageImg = loadImage("assets\\rabit1.png");
    d->rabit.loseImg = loadImage("assets\\rabit2.png");
    d->rabit.winImg = loadImage("assets\\rabit3.png");

    d->gori.normalImg = loadImage("assets\\gorilla0.png");
    d->gori.damageImg = loadImage("assets\\gorilla1.png");
    d->gori.loseImg = loadImage("assets\\gorilla2.png");
    d->gori.winImg = loadImage("assets\\gorilla3.png");
    
    d->unko.img = loadImage("assets\\unko0.png");
    
    d->ninjin.img = loadImage("assets\\ninjin0.png");
    
    d->titleImg = loadImage("assets\\title.png");
    
    d->backImg = loadImage("assets\\back.png");
}
void title(struct DATA* d) {
    clear();
    rectMode(CORNER);
    imageColor(255, 255, 255);
    image(d->titleImg, 0, 0);
    if (isTrigger(KEY_SPACE)) {
        init(d);
        //ステート切り替え
        d->state = d->PLAY;
    }
}
void init(struct DATA* d) {
    d->rabit.img = d->rabit.normalImg;
    d->rabit.px = 150.0f;
    d->rabit.py = 400.0f;
    d->rabit.vx = 4.0f;
    d->rabit.hp = 50;
    d->rabit.bulletOfsY = -70;
    d->rabit.hpGaugeOfsY = -60;

    d->gori.img = d->gori.normalImg;
    d->gori.px = 150.0f;
    d->gori.py = 70.0f;
    d->gori.vx = 4.0f;
    d->gori.bulletOfsY = 55;
    d->gori.hp = 100;
    d->gori.hpGaugeOfsY = -62;
    
    d->unko.px = 0.0f;
    d->unko.py = 0.0;
    d->unko.vy = 5.0f;
    d->unko.hp = 0;
    
    d->ninjin.px = 0.0f;
    d->ninjin.py = 0.0f;
    d->ninjin.vy = -5.0f;
    d->ninjin.hp = 0;
    
    //当たり判定用データ
    d->rabit.halfW = 35;
    d->rabit.halfH = 55;
    d->gori.halfW = 40;
    d->gori.halfH = 60;
    d->unko.halfW = 20;
    d->unko.halfH = 16;
    d->ninjin.halfW = 10;
    d->ninjin.halfH = 16;

    d->frameRestrictInput = 90;
}
void play(struct DATA* d) {
    
    //ウサギを動かす
    if (isPress(KEY_A)) { d->rabit.px += -d->rabit.vx; }
    if (isPress(KEY_D)) { d->rabit.px += d->rabit.vx; }
    //ウサギの移動範囲制限
    if (d->rabit.px < d->rabit.halfW) { 
        d->rabit.px = d->rabit.halfW; 
    }
    if (d->rabit.px > width - d->rabit.halfW) {
        d->rabit.px = width - d->rabit.halfW;
    }
    
    //ニンジンを発射する
    if (d->ninjin.hp == 0 && isTrigger(KEY_SPACE)) {
        //発射位置
        d->ninjin.px = d->rabit.px;
        d->ninjin.py = d->rabit.py + d->rabit.bulletOfsY;
        d->ninjin.hp = 1;
    }
    //ニンジンを動かす
    if (d->ninjin.hp > 0) {
        d->ninjin.py += d->ninjin.vy;
        //ウィンドウの外に出た
        if (d->ninjin.py < -d->ninjin.halfH) {
            d->ninjin.hp = 0;
        }
    }

    //ゴリラを動かす
    d->gori.px += d->gori.vx;
    if (d->gori.px < d->gori.halfW ||
        d->gori.px > width - d->gori.halfW) { 
        //移動方向反転
        d->gori.vx = -d->gori.vx; 
    }

    //ウンコする
    if (d->unko.hp == 0) {
        //発射位置
        d->unko.px = d->gori.px;
        d->unko.py = d->gori.py + d->gori.bulletOfsY;
        d->unko.hp = 1;
    }
    //ウンコを動かす
    if (d->unko.hp > 0) {
        d->unko.py += d->unko.vy;
        //ウィンドウの外に出た
        if (d->unko.py > height + d->unko.halfH) {
            d->unko.hp = 0;
        }
    }

    //当たり判定・ウサギとうんこ
    if (collision(&d->rabit, &d->unko)) {
        d->rabit.img = d->rabit.damageImg;
        d->rabit.hp--;
    }
    else {
        d->rabit.img = d->rabit.normalImg;
    }
    //当たり判定・ゴリラとにんじん
    if (collision(&d->gori, &d->ninjin)) {
        d->gori.img = d->gori.damageImg;
        d->gori.hp--;
    }
    else {
        d->gori.img = d->gori.normalImg;
    }

    //描画
    draw(d);

    //勝負がついた
    if (d->rabit.hp <= 0 || d->gori.hp <= 0) {
        //勝ち負け画像切り替え
        if (d->gori.hp > 0) {//ゴリラが勝った
            d->gori.img = d->gori.winImg;
            d->rabit.img = d->rabit.loseImg;
        }
        else {//ラビットが勝った
            d->rabit.img = d->rabit.winImg;
            d->gori.img = d->gori.loseImg;
        }
        //ステート切り替え
        d->frameCnt = d->frameRestrictInput;
        d->state = d->RESULT;
    }

}
int collision(struct CHARA* c, struct BULLET* b) {
    //矩形と矩形の当たり判定
    if (b->hp > 0) {
        float cRight = c->px + c->halfW;
        float cLeft = c->px - c->halfW;
        float cTop = c->py - c->halfH;
        float cBottom = c->py + c->halfH;
        float bRight = b->px + b->halfW;
        float bLeft = b->px - b->halfW;
        float bTop = b->py - b->halfH;
        float bBottom = b->py + b->halfH;
        if (cRight < bLeft || bRight < cLeft ||
            bBottom < cTop || cBottom < bTop ) {
            //触れていない
            return 0;
        }
        else {
            //触れている
            return 1;
        }
    }
    return 0;
}
void draw(struct DATA* d) {
    clear();
    //背景
    rectMode(CORNER);
    image(d->backImg, 0, 0);
    //キャラ
    rectMode(CENTER);
    image(d->rabit.img, d->rabit.px, d->rabit.py);
    image(d->gori.img, d->gori.px, d->gori.py);
    //弾
    if (d->ninjin.hp > 0) {
        image(d->ninjin.img, d->ninjin.px, d->ninjin.py);
    }
    if (d->unko.hp > 0) {
        image(d->unko.img, d->unko.px, d->unko.py);
    }
    //ＨＰゲージ
    hpGauge(&d->gori);
    hpGauge(&d->rabit);
#if _DEBUG
    //当たり判定エリア表示
    fill(255, 255, 255, 128);
    rect(d->rabit.px, d->rabit.py, d->rabit.halfW * 2, d->rabit.halfH * 2);
    rect(d->unko.px, d->unko.py, d->unko.halfW * 2, d->unko.halfH * 2);
    rect(d->gori.px, d->gori.py, d->gori.halfW * 2, d->gori.halfH * 2);
    rect(d->ninjin.px, d->ninjin.py, d->ninjin.halfW * 2, d->ninjin.halfH * 2);
#endif
}
void hpGauge(struct CHARA* c) {
    strokeWeight(0);
    if (c->hp > 15) {
        fill(0, 255, 0);
    }
    else {
        fill(255, 0, 0);
    }
    rect(c->px, c->py + c->hpGaugeOfsY, c->hp, 5);
}
void result(struct DATA* d) {
    //ニンジンとウンコを画面外まで移動
    if (d->ninjin.hp > 0) {
        d->ninjin.py += d->ninjin.vy;
        if (d->ninjin.py < -d->ninjin.halfH) {
            d->ninjin.hp = 0;
        }
    }
    if (d->unko.hp > 0) {
        d->unko.py += d->unko.vy;
        if (d->unko.py > height + d->unko.halfH) {
            d->unko.hp = 0;
        }
    }
    //描画
    draw(d);

    //リスタート(数フレームループしないとスペースキーを押せない)
    if (d->frameCnt > 0) {
        d->frameCnt--;
    }
    else {
        text("Spaceでタイトルに戻ります", 25, height);
        if (isTrigger(KEY_SPACE)) {
            d->state = d->TITLE;
        }
    }
}

void gmain() {
    window(300, 480);
    //全データ
    struct DATA d;
    //全画像読み込み
    loading(&d);
    ShowCursor(false);
    while (notQuit) {
        //ゲームステート制御
        if      (d.state == d.TITLE ) { title(&d); }
        else if (d.state == d.PLAY  ) { play(&d); }
        else if (d.state == d.RESULT) { result(&d); }
    }
    ShowCursor(true);
}
#endif