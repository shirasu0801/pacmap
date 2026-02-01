# PACMAP (C++ / WebAssembly)

レトロなパックマンスタイルのアーケードゲームをC++で実装し、WebAssembly (WASM) にコンパイルしてブラウザで動作させるプロジェクトです。

## 目次

1. [アプリケーション要件](#アプリケーション要件)
2. [事前準備とインストール](#事前準備とインストール)
3. [設定作業](#設定作業)
4. [実行方法](#実行方法)
5. [フォルダ構造](#フォルダ構造)
6. [各ファイルの解説](#各ファイルの解説)
7. [改善ポイント](#改善ポイント)
8. [ネクストアクション](#ネクストアクション)

---

## アプリケーション要件

### 機能要件

| 項目 | 内容 |
|------|------|
| ゲームタイプ | パックマンクローン（アーケードゲーム） |
| ステージ数 | 3面 |
| 敵の数 | 各ステージ3体 |
| パワーエサ | あり（効果時間: 7秒） |
| ワープ通路 | あり（左右に通過可能、敵は使用不可） |
| 操作方法 | 矢印キー または WASD |
| スコアシステム | エサ: 10点、パワーエサ: 50点、敵を食べる: 200点 |

### 非機能要件

| 項目 | 内容 |
|------|------|
| 動作環境 | モダンブラウザ（Chrome, Firefox, Edge, Safari） |
| フレームレート | 60fps |
| レスポンシブ | デスクトップ向け（380x420 キャンバス） |
| 技術スタック | C++17 → WebAssembly (Emscripten) + HTML/CSS/JavaScript |

---

## 事前準備とインストール

### 必要なソフトウェア

1. **Emscripten SDK (emsdk)**
   - C++をWebAssemblyにコンパイルするためのツールチェーン
   - バージョン: 3.0以上推奨

2. **Python 3**
   - ローカル開発サーバー用
   - バージョン: 3.7以上

3. **Webブラウザ**
   - Chrome, Firefox, Edge, Safari のいずれか

### Emscripten のインストール

```bash
# Windows の場合
cd C:\
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
emsdk install latest
emsdk activate latest

# 環境変数の設定（毎回必要、または永続化）
emsdk_env.bat
```

```bash
# macOS / Linux の場合
cd ~
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

### プロジェクトのクローン

```bash
git clone <repository-url>
cd pacmap
```

---

## 設定作業

### 1. build.bat の確認（Windows）

`build.bat` 内の `EMCC` パスが正しいことを確認してください：

```batch
set EMCC=C:\emsdk\upstream\emscripten\emcc.bat
```

別の場所にインストールした場合は、パスを修正してください。

### 2. ゲームパラメータの調整

必要に応じて以下のファイルでパラメータを調整できます：

**src/player.h**
```cpp
static constexpr float MOVE_SPEED = 0.08f;  // プレイヤー移動速度
static const int POWER_DURATION = 420;       // パワーエサ効果時間（7秒 = 420フレーム）
static const int SPAWN_PROTECTION_TIME = 120; // スポーン保護時間（2秒）
```

**src/enemy.h**
```cpp
static const int CHASE_THRESHOLD = 8;    // 追跡開始距離
static const int EATEN_DURATION = 180;   // 食べられた後の逃走時間（3秒）
static const int RECOVERY_DURATION = 60; // 復活後の無敵時間（1秒）
```

**src/game.cpp**
```cpp
float baseSpeed = 0.05f + (currentStage - 1) * 0.01f;  // 敵の基本速度
```

---

## 実行方法

### ビルド

```bash
# Windows
build.bat

# macOS / Linux
chmod +x build.sh
./build.sh
# または直接:
emcc src/game.cpp src/player.cpp src/enemy.cpp src/map.cpp src/bindings.cpp \
  -o web/pacmap.js -std=c++17 -O2 -sWASM=1 \
  -sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
  -sALLOW_MEMORY_GROWTH=1 -sMODULARIZE=1 \
  -sEXPORT_NAME=createModule -sENVIRONMENT=web \
  -I src
```

### ローカルサーバーの起動

```bash
cd web
python -m http.server 8080
```

### ブラウザでアクセス

```
http://localhost:8080
```

### 操作方法

| キー | アクション |
|------|----------|
| Space | ゲーム開始 / リスタート |
| ↑ / W | 上に移動 |
| ↓ / S | 下に移動 |
| ← / A | 左に移動 |
| → / D | 右に移動 |

---

## フォルダ構造

```
pacmap/
├── src/                    # C++ソースコード
│   ├── game.cpp           # ゲームメインロジック
│   ├── game.h             # ゲームクラス定義
│   ├── player.cpp         # プレイヤー処理
│   ├── player.h           # プレイヤークラス定義
│   ├── enemy.cpp          # 敵AI（徘徊・追跡・逃走）
│   ├── enemy.h            # 敵クラス定義
│   ├── map.cpp            # マップデータ・当たり判定
│   ├── map.h              # マップクラス定義
│   └── bindings.cpp       # JavaScript連携用エクスポート関数
├── web/                    # フロントエンド
│   ├── index.html         # メインHTML
│   ├── style.css          # サイバーパンク風CSS
│   ├── game.js            # ゲームレンダリング・入力処理
│   ├── pacmap.js          # Emscripten生成グルーコード
│   └── pacmap.wasm        # コンパイル済みWebAssembly
├── build.bat              # Windows用ビルドスクリプト
└── README.md              # このファイル
```

---

## 各ファイルの解説

### C++ コア (src/)

#### game.h / game.cpp - ゲーム状態管理

```cpp
// ゲーム状態の列挙型
enum class GameState {
    TITLE,        // タイトル画面
    PLAYING,      // プレイ中
    STAGE_CLEAR,  // ステージクリア
    GAME_CLEAR,   // 全ステージクリア
    GAME_OVER     // ゲームオーバー
};
```

**主な機能:**
- `init()`: ゲーム初期化
- `startGame()`: ゲーム開始
- `update()`: 毎フレームの更新処理
- `checkCollisions()`: 当たり判定（float座標ベース、閾値0.3タイル）
- `checkDotCollection()`: エサ収集判定
- `resetPositions()`: プレイヤーと敵の位置リセット

**敵の追跡ロジック:**
```cpp
// 最も近い敵だけが追跡可能
int closestEnemyIndex = -1;
int closestDistance = 9999;
for (int i = 0; i < NUM_ENEMIES; i++) {
    if (!enemies[i].isEaten()) {
        int dist = /* マンハッタン距離計算 */;
        if (dist < closestDistance) {
            closestDistance = dist;
            closestEnemyIndex = i;
        }
    }
}
enemies[i].setCanChase(i == closestEnemyIndex);
```

#### player.h / player.cpp - プレイヤー制御

**主な機能:**
- `update()`: 移動処理、ワープ通路対応
- `setDirection()`: 入力方向のバッファリング
- `canMove()`: 指定方向への移動可否判定
- `activatePowerMode()`: パワーモード発動

**移動処理の流れ:**
1. タイル中央判定（閾値 < 0.05）
2. バッファされた入力方向への切り替え試行
3. 壁判定で移動可否チェック
4. ワープ通路での座標ワープ処理

#### enemy.h / enemy.cpp - 敵AI

**敵の状態:**
```cpp
enum class EnemyState {
    WANDER,      // 徘徊（ランダム移動）
    CHASE,       // 追跡（プレイヤー方向へ）
    FRIGHTENED,  // 怯え（プレイヤーから逃げる）
    EATEN        // 食べられた（逃走中）
};
```

**AI行動パターン:**
- **WANDER**: ランダムな方向に移動、行き止まりで反転
- **CHASE**: プレイヤーとの距離が閾値以下で発動、最短経路で追跡
- **FRIGHTENED**: パワーエサ中、プレイヤーから最も遠い方向へ逃走
- **EATEN**: 食べられた後、一定時間逃走してから通常に復帰

#### map.h / map.cpp - マップ管理

**タイルタイプ:**
```cpp
enum class TileType {
    EMPTY = 0,        // 空（ワープ通路用）
    WALL = 1,         // 壁
    DOT = 2,          // エサ
    POWER_PELLET = 3, // パワーエサ
    GATE = 4          // ゲート（未使用）
};
```

**主な機能:**
- `loadStage()`: ステージデータの読み込み
- `isWalkable()`: プレイヤー用通行判定
- `isWalkableForEnemy()`: 敵用通行判定（ワープ通路をブロック）
- `wrapX()`: 水平方向のワープ座標変換

#### bindings.cpp - JavaScript連携

Emscriptenの `EMSCRIPTEN_KEEPALIVE` マクロで関数をエクスポート：

```cpp
extern "C" {
    EMSCRIPTEN_KEEPALIVE void game_init();
    EMSCRIPTEN_KEEPALIVE void game_start();
    EMSCRIPTEN_KEEPALIVE void game_update();
    EMSCRIPTEN_KEEPALIVE void game_set_input(int direction);
    EMSCRIPTEN_KEEPALIVE float game_get_player_x();
    // ... 他のゲッター関数
}
```

### フロントエンド (web/)

#### index.html - メインHTML

- Canvas要素（380x420ピクセル）
- オーバーレイUI（タイトル、ゲームオーバー、クリア画面）
- スコア、ステージ、ライフ表示
- WASMモジュールとgame.jsの読み込み

#### style.css - スタイリング

- サイバーパンク風のダークテーマ
- ネオンカラー（緑、青、紫）
- Press Start 2P フォント（レトロゲーム風）
- 点滅アニメーション

#### game.js - ゲームロジック（JavaScript側）

**主な機能:**
- `init()`: WASMモジュールの非同期ロード
- `setupInput()`: キーボード入力ハンドリング
- `gameLoop()`: 60fpsのレンダリングループ
- `render()`: Canvas描画
- `drawPlayer()`: プレイヤー描画（サイバー風の矢印形状）
- `drawEnemy()`: 敵描画（六角形のロボット）
- `drawTile()`: マップタイル描画

**キャラクターデザイン:**
- プレイヤー: サイバーグリーンの矢印/宇宙船形状
- 敵: 各色（赤、紫、オレンジ）の六角形ロボット
- 怯え状態: 青色、目が恐怖表現に変化

---

## 改善ポイント

### 機能追加

1. **サウンド効果**
   - Web Audio API を使用したBGMと効果音
   - エサ収集音、パワーエサ音、敵を食べる音、ミス音

2. **モバイル対応**
   - タッチ操作（スワイプで方向入力）
   - レスポンシブデザイン

3. **追加ゲーム要素**
   - ボーナスアイテム（フルーツ）
   - ステージ間のカットシーン
   - ハイスコア保存（LocalStorage）

4. **敵AIの改善**
   - オリジナルパックマンのゴースト個性（追跡、待ち伏せ、ランダム、逃走）
   - より高度な経路探索（A*アルゴリズム）

### 技術的改善

1. **パフォーマンス最適化**
   - スプライトシートの使用
   - オフスクリーンキャンバスでの事前レンダリング
   - RequestAnimationFrameのタイムスタンプベース更新

2. **コード品質**
   - TypeScriptへの移行
   - ユニットテストの追加
   - ESLint / Prettier の導入

3. **ビルドシステム**
   - CMakeの活用
   - CI/CD パイプライン（GitHub Actions）

---

## ネクストアクション

### インフラへのデプロイ

#### 1. 静的ホスティング（推奨）

**GitHub Pages:**
```bash
# リポジトリ設定で GitHub Pages を有効化
# web/ フォルダを公開ディレクトリに設定
git add .
git commit -m "Deploy to GitHub Pages"
git push origin main
```

**Netlify:**
```bash
# netlify.toml を作成
[build]
  publish = "web"
  command = "echo 'Static site - no build needed'"

# Netlify CLIでデプロイ
npm install -g netlify-cli
netlify deploy --prod --dir=web
```

**Vercel:**
```bash
npm install -g vercel
cd web
vercel --prod
```

**AWS S3 + CloudFront:**
```bash
# S3バケットの作成と静的ホスティング有効化
aws s3 mb s3://pacmap-game
aws s3 website s3://pacmap-game --index-document index.html

# ファイルのアップロード
aws s3 sync web/ s3://pacmap-game --acl public-read

# CloudFrontディストリビューションの作成（HTTPS対応）
```

#### 2. Docker化（オプション）

```dockerfile
# Dockerfile
FROM nginx:alpine
COPY web/ /usr/share/nginx/html/
EXPOSE 80
CMD ["nginx", "-g", "daemon off;"]
```

```bash
docker build -t pacmap .
docker run -p 8080:80 pacmap
```

#### 3. Kubernetes デプロイ（大規模運用）

```yaml
# k8s/deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: pacmap
spec:
  replicas: 2
  selector:
    matchLabels:
      app: pacmap
  template:
    metadata:
      labels:
        app: pacmap
    spec:
      containers:
      - name: pacmap
        image: pacmap:latest
        ports:
        - containerPort: 80
---
apiVersion: v1
kind: Service
metadata:
  name: pacmap-service
spec:
  type: LoadBalancer
  ports:
  - port: 80
    targetPort: 80
  selector:
    app: pacmap
```

### 運用

#### モニタリング

1. **Google Analytics**
   - ユーザー数、セッション時間、離脱率

2. **エラー監視**
   - Sentry でJavaScriptエラーをトラッキング

3. **パフォーマンス監視**
   - Lighthouse CI でパフォーマンススコアを継続監視

#### セキュリティ

1. **HTTPSの強制**
   - SSL/TLS証明書の設定（Let's Encrypt）

2. **コンテンツセキュリティポリシー (CSP)**
   ```html
   <meta http-equiv="Content-Security-Policy"
         content="default-src 'self'; script-src 'self' 'wasm-unsafe-eval';">
   ```

3. **定期的な依存関係の更新**
   - Emscriptenの最新版への追従

#### バックアップと復旧

1. **ソースコードのバージョン管理**
   - Git + リモートリポジトリ（GitHub, GitLab）

2. **ビルド成果物のアーカイブ**
   - リリースタグごとのWASMファイル保存

---

## ライセンス

MIT License

## 作者

このプロジェクトはClaude（Anthropic）のアシスタントと共に開発されました。

---

## トラブルシューティング

### ビルドエラー

**emccが見つからない:**
```bash
# emsdkの環境変数を設定
source ~/emsdk/emsdk_env.sh  # Linux/macOS
C:\emsdk\emsdk_env.bat       # Windows
```

**WASMが更新されない:**
```bash
# 古いファイルを削除してリビルド
rm web/pacmap.js web/pacmap.wasm
./build.bat  # または build.sh
```

### ランタイムエラー

**WASMモジュールが読み込めない:**
- ローカルファイルではなくHTTPサーバー経由でアクセス
- ブラウザのキャッシュをクリア（Ctrl+Shift+R）

**入力が反応しない:**
- Canvasにフォーカスがあることを確認
- ブラウザの開発者ツールでJavaScriptエラーを確認
