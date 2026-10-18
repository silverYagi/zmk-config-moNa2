"""元のQMK配列と、moNa2の物理キーに移植した配列を照合する。"""

import argparse
from pathlib import Path
import re


LAYERS = {
    "_QWERTY": "QWERTY",
    "_ONISHI": "ONISHI",
    "_SYMBOL": "SYMBOL",
    "_NUMBER_NAV": "NUMBER_NAV",
    "_FUNCTION": "FUNCTION",
}

KEYS = {
    "LCTL": "LCTRL", "RCTL": "RCTRL", "LSFT": "LSHIFT", "RSFT": "RSHIFT",
    "LGUI": "LGUI", "RGUI": "RGUI", "LALT": "LALT", "RALT": "RALT",
    "SPC": "SPACE", "ENT": "ENTER", "MINS": "MINUS", "COMM": "COMMA", "DOT": "DOT",
    "SLSH": "SLASH", "RBRC": "RIGHT_BRACKET", "LBRC": "LEFT_BRACKET",
    "BSLS": "BACKSLASH", "EQL": "EQUAL", "QUOT": "SQT", "SCLN": "SEMI",
    "BSPC": "BSPC", "DEL": "DEL", "ESC": "ESC", "TAB": "TAB",
    "HOME": "HOME", "END": "END", "LEFT": "LEFT", "RGHT": "RIGHT",
    "DOWN": "DOWN", "UP": "UP", "PGUP": "PG_UP", "PGDN": "PG_DN",
    "APP": "K_APP", "LNG1": "LANG1", "LNG2": "LANG2", "GRV": "GRAVE",
    "INS": "INSERT", "PSCR": "PSCRN", "INT3": "INT3",
    "MEDIA_PLAY_PAUSE": "C_PP", "AUDIO_VOL_UP": "C_VOL_UP",
    "AUDIO_VOL_DOWN": "C_VOL_DN",
}

# ZMKの既存PMW3610ドライバーには、これらのQMK専用操作に相当するキーがない。
UNSUPPORTED = {"KBC_SAVE", "SCRL_DVI", "SCRL_DVD", "CPI_I1K", "CPI_I100", "CPI_D100", "CPI_D1K"}

# moNa2の追加3キーと、Keyball39右側のボールに隠れる仮想3キーを区別する。
POSITIONS = (
    list(range(10))
    + list(range(10, 15)) + [None] + list(range(15, 20))
    + list(range(20, 25)) + [None, None] + list(range(25, 30))
    + list(range(30, 38)) + [41]
)


def split_arguments(source):
    arguments, start, depth = [], 0, 0
    for index, char in enumerate(source):
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
        elif char == "," and depth == 0:
            arguments.append(source[start:index].strip())
            start = index + 1
    arguments.append(source[start:].strip())
    return arguments


def keycode(source):
    if source.startswith("KC_"):
        name = source[3:]
        if name in KEYS:
            return KEYS[name]
        if len(name) == 1 and name.isdigit():
            return "N" + name
        if len(name) == 1 or re.fullmatch(r"F\d+", name):
            return name
        raise ValueError(f"未対応のキーコード: {source}")
    match = re.fullmatch(r"([SCAG])\((.*)\)", source)
    if match:
        modifier = {"S": "LS", "C": "LC", "A": "LA", "G": "LG"}[match[1]]
        return f"{modifier}({keycode(match[2])})"
    raise ValueError(f"未対応のキーコード式: {source}")


def binding(source):
    if source == "_______":
        return "&trans"
    if source in UNSUPPORTED:
        return "&none"
    if source.startswith("KC_BTN"):
        return "&mkp MB" + source[-1]
    match = re.fullmatch(r"(LCTL|RCTL|LALT|RALT|LSFT|RSFT)_T\((.*)\)", source)
    if match:
        return f"&mt {KEYS[match[1]]} {keycode(match[2])}"
    match = re.fullmatch(r"(MO|TO|TG)\((.*)\)", source)
    if match:
        behavior = {"MO": "mo", "TO": "to", "TG": "tog"}[match[1]]
        return f"&{behavior} {LAYERS[match[2]]}"
    return "&kp " + keycode(source)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("qmk_keymap", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    qmk = re.sub(r"/\*.*?\*/|//[^\n]*", "", args.qmk_keymap.read_text(encoding="utf-8"), flags=re.S)
    zmk = re.sub(r"/\*.*?\*/", "", (root / "config/moNa2.keymap").read_text(encoding="utf-8"), flags=re.S)
    qmk_layers = re.findall(r"\[(_[A-Z_]+)\]\s*=\s*LAYOUT_universal\((.*?)\n\s*\),", qmk, flags=re.S)
    zmk_keymap = zmk.split('compatible = "zmk,keymap";', 1)[1]
    zmk_layers = re.findall(r"\b(\w+)\s*\{\s*bindings\s*=\s*<(.*?)>;", zmk_keymap, flags=re.S)
    assert len(qmk_layers) == 5, "元配列は5レイヤーである必要があります"
    assert len(zmk_layers) == 6, "移植先は管理用を含む6レイヤーである必要があります"
    assert len(POSITIONS) == 42
    checked = 0
    for layer_index, (name, body) in enumerate(zmk_layers):
        actual = [" ".join(part.split()) for part in re.findall(r"&[^&]+", body)]
        assert len(actual) == 42, f"{name}: {len(actual)}キー"
        if layer_index >= 5:
            continue
        source_name, source_body = qmk_layers[layer_index]
        assert name == LAYERS[source_name].lower()
        original = split_arguments(source_body)
        assert len(original) == 42
        for target_position, source_position in enumerate(POSITIONS):
            if source_position is None:
                continue
            expected = binding(original[source_position])
            assert actual[target_position] == expected, (
                f"{name}[{target_position}] 元位置{source_position}: "
                f"{actual[target_position]} != {expected}"
            )
            checked += 1
    print(f"照合成功: 6レイヤー×42キー、元配列との比較{checked}件")


if __name__ == "__main__":
    main()
