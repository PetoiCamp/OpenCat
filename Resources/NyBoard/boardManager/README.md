# Arduino IDE Board Manager

Adds the NyBoard board definition through the *Arduino 1.6.4 (and later)* board manager rather then editing `boards.txt` directly as described at https://github.com/PetoiCamp/OpenCat/wiki/Software-setup.

## Usage

1. Open the Arduino IDE application

2. Open the `Preferences...` panel

3. In the `Additional Boards Manager URLs` field, add `https://raw.githubusercontent.com/PetoiCamp/OpenCat/master/Resources/NyBoard/boardManager/package_petoi_nyboard_index.json`. If there are already URLs listed in this field, separate them with commas or click on the icon next to the field to open up an editor and add this URL to a new line.

4. Click on the `OK` button to close the `Preferences...` panel (you may need to do this for the `Additional Boards Manager URLs` editor first, if you opened it up in the previous step)

5. Open the `Boards Manager...` window with the `Tools` -> `Board: XXXX` -> `Boards Manager...` menu item.

6. In the `Filter your search...` field, type `NyBoard`

7. Select the entry and click on `Install`

8. Click on the `Close` button

9. Select `ATmega328P (5V, 20 MHz) NyBoard` from the `Tools` -> `Board: XXXX` menu and enjoy!

## Updating the Board Definition

This section should not be necessary for most users and is included only for reference. As this is my first foray into using the Boards Manager, these steps may change slightly as necessary and I will try to keep this updated for reference.

1. Any additions to the `NyBoard-0.1` directory or changes to `NyBoard-0.1/boards.txt` require the following additional steps (update version numbers as appropriate):

  A. The archive file `NyBoard-0.1.x.tar.bz2` needs to be rebuilt

    i. Mac or Linux, use: `tar -cjf NyBoard-0.1.x.tar.bz2 NyBoard-0.1`

    ii. *TBD* need Windows equivalent

  B. Update `pacakge_petoi_nyboard_index.json` as follows:

    i. Update the `"version"` field to match the new version number (e.g. "0.1.x")

    ii. Update the `"url"` and `"archiveFileName"` fields to match the new archive name.

    iii. Update the `"size"` field to match the new archive file size

    iv. Update the `"checksum"` field to match the new archive file

      a. Mac or Linux use: `openssl dgst -sha256 NyBoard-0.1.x.tar.bz2` to get this value

      b. *TBD* need Windows equivalent

  C. Alternatively, if you wish to keep multiple versions of the board definition around, you can copy the entire entry in `"platforms"` (lines 12 - 31), add a comma after the closing brace of the last entry in `"platforms"`, paste the copy to the next line (before the closing bracket) and make the changes in **B** to the new entry.

2. Changes to the other informational fields of `pacakge_petoi_nyboard_index.json` should not require rebuilding the archive or modifying the file size or checksum values.

## Reference

* https://github.com/PetoiCamp/OpenCat/wiki/Software-setup
* https://github.com/arduino/Arduino/wiki/Arduino-IDE-1.6.x-package_index.json-format-specification
