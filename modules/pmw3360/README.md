# moNa2用PMW3360ドライバー

[george-norton/zmk-driver-pmw3360](https://github.com/george-norton/zmk-driver-pmw3360)の
`0dfa2f0411b795613a35f222f993e45a1e4cc32a`を基に、MITライセンスで同梱しています。
元の著作権表記とLICENSEを維持しています。

今回の7ピン読取基板にはMOTION端子がないため、4ms間隔のポーリングで使用します。
上流の通常workを遅延workとして扱う実装を、実際の`k_work_delayable`へ修正しました。
CSのGPIOを初期化し、負論理のHigh→Low操作に修正しています。
センサーIDが製品0x42・リビジョン0x01と一致しない場合はポーリングを開始しません。
製品IDはKeyballの元ドライバーとQMKのPMW3360定義で照合しています。
読取失敗と0xffのモーション値は入力として送信しません。

SROMの追加ダウンロードは上流同様に行いません。実機の動作は別途確認が必要です。
本体ソケットからGPIOへの対応は仮定を含みます。`docs/pmw3360-test.md`を参照してください。
