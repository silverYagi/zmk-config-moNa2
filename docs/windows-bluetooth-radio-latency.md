# BLE再起動対策の比較版

shakushakupanda氏の[test-build212](https://github.com/shakushakupanda/zmk-config-moNa2-v2/releases/tag/test-build212)に含まれる、無線イベント準備遅延時の再起動対策を移植した版です。

## 変更範囲

動作確認済みのPMW3360・DYA・Keyball39配列を含むパスキー版`efb9563`を土台に、右側へ次の2設定だけを追加します。

```conf
CONFIG_BT_CTLR_ADVANCED_FEATURES=y
CONFIG_BT_CTLR_ASSERT_OVERHEAD_START=n
```

参照した修正:

- [9078fd3: 無線準備遅延時に再起動せずスキップ](https://github.com/shakushakupanda/zmk-config-moNa2-v2/commit/9078fd328a205093b189a5b264a614e831798e80)
- [2d73c2e: 上記設定を変更可能にする](https://github.com/shakushakupanda/zmk-config-moNa2-v2/commit/2d73c2e1c504bcab5a5f725c8d9841a388d4d2ab)

#212のZMK v0.4系・Zephyr 4.1への移行、エンコーダー変更、スクロール調整は含めません。
現在のcormoran/ZMK `4493783ef88ce2e653bf8217c92ee17140df71e3`とZephyr 3.5にも同じ設定とスキップ処理が存在することを確認しています。
PMW3360のドライバー・端子・入力処理・大西配列・2M PHY無効化・パスキー認証・DYAを維持します。
USBログ採取機能や設定リセットは追加しません。

## 書き込み

1. DYAで変更を保存した後は10秒以上待ちます。
2. 右側をUSB接続し、リセットを素早く2回押します。
3. 表示されたドライブへ`mona2-right-keyball39-dya-pmw3360-ble-fix212.uf2`をコピーします。
4. 左右の電源をONにし、右のUSB接続で左右のキーとボールが動くことを確認します。

現在の左側は右のUSB接続で入力できているため、左の書き直しは不要です。
同梱の`mona2-left-keyball39-dya.uf2`は再書き込みが必要になった場合の予備です。
`settings_reset`は使用しません。保存済みのDYA配列とスクロール方向を引き継ぎます。

## Windows接続確認

WindowsにmoNa2の登録が残っていれば削除します。
DYAでWindows用の空きBluetoothプロファイルを選びます。既存のWindows用を使う場合はそのプロファイルだけをクリアし、10秒待ちます。
DYAの接続を切断し、左右の電源をONにした状態で右USBを抜いて、Windowsの「デバイスの追加 → Bluetooth」からmoNa2を選びます。

6桁の番号が出た場合はmoNa2自身で入力してEnterを押します。数字キーの位置は[パスキー版の説明](windows-bluetooth-passkey.md#数字の入力位置)を参照してください。
USB給電を続けて試す場合、出力先はBluetooth管理キー＋Wの位置でBluetoothへ切り替えます。USB出力へ戻す操作は管理キー＋Qの位置です。

まず「6桁が表示される前の失敗が変わったか」を確認します。
接続できた場合は、使用中に一時停止やLEDの起動表示を伴う再起動が起きないかを確認します。
今回の対策は再起動を避けるもので、通信遅延自体を解消するものではありません。遅延が連続すると接続が切れる可能性は残ります。
また、初回ペアリング失敗の原因が同じかは未確認です。Windowsでの成功は実機で確認する必要があります。

## 元に戻す

右に同梱の`mona2-right-keyball39-dya-pmw3360-passkey.uf2`を上書きします。
両方とも同じパスキー認証方式です。設定リセットや左の書き換えは不要です。
