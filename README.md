# FingerVision用RTコンポーネント

大阪電気通信大学  
升谷 保博

## 概要
- [FingerVision](http://akihikoy.net/p/fv.html)用の[ソフトウェア](https://github.com/akihikoy/fingervision)に含まれるROS用のノードをRTミドルウェアのコンポーネントとして移植したものです．
- 以下のディレクトリは，オリジナルをそのままコピーしました（Windowsに対応するために一部を変更しています）
  - [3rdparty/ay_cpp](https://github.com/akihikoy/fingervision/tree/master/3rdparty/ay_cpp) → [ay_cpp](ay_cpp/)
  - [3rdparty/ay_vision](https://github.com/akihikoy/fingervision/tree/master/3rdparty/ay_vision) → [ay_vision](ay_vision/)
  - [fingervision/config](https://github.com/akihikoy/fingervision/tree/master/fingervision/config) → [config](config/)
  - [fv_core](https://github.com/akihikoy/fingervision/tree/master/fv_core) → [fv_core](fv_core/)
- 以下で動作を確認
  - Windows 10 64bit
  - Visual Studio 2015 C++ x64 Release
  - OpenRTM-aist 1.2.0 64bit
  - OpenCV 2.4.13.6
  - Eigen 3.3.7
  - Boost 1.69
- Ubuntu 16.04でもビルド・実行できることを簡単に確認しています．

## インストール（Windowsの場合）

- OpenRTM-aistをインストール．
- OpenCV 2系, Eigen, Boostをインストール．
- [FingerVisionCore](https://github.com/MasutaniLab/FingerVisionCore)をクローン．
- CMake
  - Configure
  - 必要に応じて以下の変数を設定．
    - Boost_INCLUDE_DIR（Boostのインストールされたディレクトリ，例：C:/boost/boost_1_69_0）
    - OpenCV_DIR（OpenCV 2系のディレクトリ，例：C:/opencv/opencv-2.4.13.6/build）
    - EIGEN_DIR（Eigenののインストールされたディレクトリ，例：C:/eigen/eigen-3.3.7）
  - エラーが出なくなるまでConfigureを繰り返す．
  - Generate
  - Open Project
- Visual Studioでx64 Releaseでソリューションをビルド．

## 仕様

独自のデータ型とサービスポートのインタフェースを[idl/FingerVision.idl](idl/FingerVision.idl)で定義しています．

### 出力ポート
各メンバの意味はオリジナルのトピックとほぼ同じです．`stamp`だけが，RTミドルウェアの慣習に従って`tm`に置き換えています．
- blob_moves0 （データ型： FingerVision::BlobMoves）
- prox_vision0 （データ型： FingerVision::ProxVision）
- blob_moves1 （データ型： FingerVision::BlobMoves）
- prox_vision1 （データ型： FingerVision::ProxVision）

### サービスポート

- service
  - インタフェース名 coreService （型： FingerVision::CoreService）
    - void pause()
      - 処理の一時停止
    - void resume()
      - 処理の再開
    - void set_frame_skip(in long num)
      - フレームスキップ数の設定
    - void stop_detect_obj()
      - 物体検知（近接視覚）を停止
    - void start_detect_obj()
      - 物体検知（近接視覚）を開始
    - void clear_obj()
      - 物体モデル（近接視覚）を消去

### コンフィギュレーション
- pkg_dir
  - 説明： 設定ファイルの親ディレクトリ（オリジナルでは，ROSのパッケージディレクトリのパス）
  - 型： string
  - デフォルト値： .

- cam_config
  - 説明： カメラの設定ファイルのパス（pkg_dirに対する相対パス）
  - 型： string
  - デフォルト値： config/cam1.yaml

- blobtrack_config
  - 説明： マーカ追跡の設定ファイルのパス（pkg_dirに対する相対パス）
  - 型： string
  - デフォルト値： config/cam1.yaml

- objdettrack_config
  - 説明： 近接視覚の設定ファイルのパス（pkg_dirに対する相対パス）
  - 型： string
  - デフォルト値： config/cam1.yaml

- blob_calib_prefix
  - 説明： マーカ追跡の校正を保存するファイル名の先頭部分
  - 型： string
  - デフォルト値： blob_

- vout_base
  - 説明： 動画を保存するファイル名の先頭部分
  - 型： string
  - デフォルト値： vout-

- frame_skip
  - 説明： 1+frame_skip毎に動画像処理する（0ならばスキップなしで毎フレーム処理）
  - 型： int
  - デフォルト値： 0

- target_fps
  - 説明： このFPSで動画像を処理する（0ならばFPS制御しない）
  - 型： double
  - デフォルト値： 0

## 使い方（Windowsの場合）

- Naming Serviceを起動しておきます．
- 設定ファイルを準備．ディレクトリ[config](config/)の下にオリジナルからコピーしたYAMLファイルがあります．
- トップディレクトリの[FingerVisionCore.conf](FingerVisionCore.conf)を編集し，設定ファイルを指定．
- トップディレクトリの[FingerVisionCore.bat](FingerVisionCore.bat)を実行．
- キー操作（必ずウィンドウをクリックしてから）
  - ESC または q: ディアクティベート
  - W: 動画ファイル出力開始・停止
  - m: 明るさ調整（ObjDetTrackerのみ）
  - スペース: 処理の一時停止・再開
  - c: キャリブレーション実行
  - r: 物体のクリア（ObjDetTrackerのみ）
  - d: 物体検出モードの切り替え
  - s: キャリブレーションデータの書き出し
  - l: キャリブレーションデータの読み込み
  - C: 閾値を調整するバーの表示・非表示の切り替え（BlobTrackerのみ）
- マウス操作
  - 右クリック： 処理の一時停止・再開
  - Shiftキー＋左クリック： 物体モデルの変更（ObjDetTrackerのみ）

## オリジナルとの違い

- オリジナルのノードでは，設定ファイルに応じでパブリッシュできるメッセージの数を任意に設定できますが，このコンポーネントの出力ポートの数は2個に固定されています．設定ファイルに応じてカメラ1台分か2台分の出力ができます．3台以上には対応していません．RTミドルウェアでも動的にポートを作ることができるので，オリジナルと同じようにすることは可能なはずです．

- ESCまたはｑキーを押すと，オリジナルではプログラムを終了しますが，このコンポーネントではディアアクティベートするだけです．そこから再度アクティベートが可能です．

以上．
