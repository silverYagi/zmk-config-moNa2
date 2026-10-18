# Keyball39配列のmoNa2移植版

現在はDYA Studio対応のキーボード確認版と、PMW3360トラックボール試験版をビルドします。
試験版の書き込み方法・端子対応の仮定は[PMW3360試験版の説明](pmw3360-test.md)を参照してください。
Keyball39の読取基板はPMW3360用で、以前のPMW3610用とはドライバーが違います。
PMW3360試験版は既存のMOTION端子をMISO入力として使用します。実配線と電源の確認は残っています。

個人用Keyball39の`keymaps/via/keymap.c`を基に、物理的に存在する39キーをmoNa2の対応する位置へ移植しています。
元のKeyball39と同じ日本語／英語キーボード設定をWindowsでも使ってください。特に記号キーは元と同じHIDコードを送ります。

| 番号 | レイヤー | 開き方 |
| --- | --- | --- |
| 0 | QWERTY | 起動時の既定。大西配列の右下キーでも戻れます |
| 1 | 大西配列 | QWERTYの右下キーで切り替え |
| 2 | 記号・マウス | 左親指の記号キーを長押し。センサー有効化後はボール操作でも一時的に有効 |
| 3 | 数字・カーソル | 右親指のレイヤーキーを長押し |
| 4 | ファンクション | 左親指の最も内側のキーを長押し。センサー有効化後はトラックボールがスクロールになります |
| 5 | Bluetooth管理 | 左手下段のBの右にある追加キーを長押し |

QWERTYと大西配列は、ホーム行のCtrl／Alt／Shift長押し、Shift／Space、親指のレイヤー操作を元と揃えています。
追加した右手の内側2キーは、上からBackspaceとDeleteです。記号レイヤーでDeleteの位置はアプリケーションメニューになります。
左のロータリーエンコーダーは通常スクロール、ファンクションレイヤーでは音量調整です。

## Bluetooth管理

左手のBの右の追加キーを押しながら、次を操作します。QWERTYと大西のどちらでも同じ物理位置です。

| 押す位置（QWERTYでの表示） | 動作 |
| --- | --- |
| 左上Q | USBへ入力を送る |
| 左上W | Bluetoothへ入力を送る |
| 右上Y／U／I／O／P | Bluetooth接続先0／1／2／3／4を選択 |
| 右下の文字キー `/` | 選択中の接続先のペアリング情報を消去 |
| 右手ホーム行いちばん右（通常はMINUS） | DYA Studioの編集ロックを解除 |

新しくWindowsへ接続するときは空き接続先を選びます。登録済みの接続先を再利用するなら、その接続先の情報をキーボードとWindowsの両方で削除します。
USB給電中にBluetoothで入力する場合は「管理キー＋W」で出力を切り替えます。USBへ戻す操作は「管理キー＋Q」です。
Windowsとの互換性確認用に、右側の`CONFIG_BT_CTLR_PHY_2M=n`を設定しています。実機での接続成功は別途確認が必要です。

## Keyball固有機能の違い

元配列のCPI増減・スクロール除数の増減・`KBC_SAVE`は、QMK固有の操作なので無効キーにしています。
センサー有効化後の感度、スクロール量、XY入れ替え、自動マウス層はDYA Studioの入力プロセッサー設定で調整します。
PMW3360の初期CPIは右側overlayの`cpi = <500>`で、元のKeyball39の初期値と揃えています。

## DYA StudioへのUSB接続

1. 右側をPCへUSB接続します。
2. ChromeまたはEdgeで[DYA Studio](https://studio.dya.cormoran.works/)を開きます。
3. 左手のBの右の管理キーを押しながら、右手ホーム行いちばん右のMINUSキーを押して編集ロックを解除します。
4. StudioのUSBマークを選び、moNa2に接続します。
5. キーマップ、Bluetooth接続先、入力プロセッサー設定を確認します。設定保存後は10秒待ってから電源を切ります。

DYAのPMW3610専用センサー診断はPMW3360には使えません。共通のマウス感度・軸・レイヤー処理と、センサー専用診断を区別してください。

## ビルドと書き込み

GitHubへブランチをpushするとActionsが左右と設定リセット用をビルドします。成果物名は次のとおりです。

| ファイル | 用途 |
| --- | --- |
| `mona2-right-keyball39-dya-pmw3360-test.uf2` | 右側、PMW3360有効。配線仮定付きの試験版 |
| `mona2-right-keyball39-dya-keyboard-only.uf2` | 右側、DYAとキー入力の確認用。ボールは無効 |
| `mona2-left-keyball39-dya.uf2` | 左側、DYA版との組み合わせ用 |
| `mona2-settings-reset-dya.uf2` | 左右共通の設定リセット |

左右は同じ実行の成果物を使用してください。この構成は`seeeduino_xiao_ble`用です。
`keyboard-only`の確認用UF2ではボールは動きません。ボールの確認には`pmw3360-test`を使用します。

初回に設定リセットする場合は、既存のペアリング情報がすべて消えます。

1. 必要な設定を控えてから、左右の電源をOFFにします。
2. 左をUSB接続し、リセットボタンを素早く2回押します。表示されたXIAOのドライブへリセット用UF2をコピーし、数秒待ってUSBを外します。
3. 右にも同じリセット用UF2を書き込み、USBを外します。
4. 左に左用UF2、右に右用UF2を、それぞれリセットボタン2回で表示したドライブへコピーします。
5. 左、右の順で電源をONにし、右のUSB接続でキー入力を確認します。
6. Windowsに古いmoNa2の登録があれば削除し、キーボードで空き接続先を選んで再ペアリングします。

元配列との照合は、元のQMKファイルを指定して実行できます。

```powershell
python tools/validate_port.py "元のKeyball39のkeymap.cへのパス"
```

## PMW3360の配線について

[Keyball39の公式ガイド](https://github.com/Yowkees/keyball/blob/main/keyball39/doc/rev1/buildguide_jp.md)では読取りICをPMW3360としています。
[採用ドライバー](https://github.com/george-norton/zmk-driver-pmw3360)はSCK、MOSI、MISO、CSを別々に設定します。

必要なのは読取基板の各信号が接続されたXIAOのGPIO番号と、電源・GNDの接続です。MOTION線は任意です。
moNa2標準のPMW3610設定ではMOSIとMISOがP0.04を共用していたため、そのまま置き換えることはできません。

写真の端子表記を基に`pmw3360-wiring.dtsi`へ試験用の対応を記述し、右側ビルドへ次を指定しています。

```yaml
cmake-args: '-DEXTRA_CONF_FILE=../../config/pmw3360.conf'
artifact-name: mona2-right-keyball39-dya-pmw3360-test
```

設定上のGPIOはキー行列と競合しません。配線の対応とセンサーのID・動作は実機確認が必要です。
