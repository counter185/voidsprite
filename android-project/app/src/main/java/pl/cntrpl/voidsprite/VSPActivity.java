package pl.cntrpl.voidsprite;

import android.Manifest;
import android.app.Activity;
import android.app.Application;
import android.content.ComponentName;
import android.content.pm.ActivityInfo;
import android.net.Uri;
import android.os.Build;
import android.content.Intent;
import android.os.Environment;
import android.os.Looper;
import android.provider.DocumentsContract;
import android.provider.Settings;
import android.util.Pair;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.content.PermissionChecker;

import com.jaredrummler.android.device.DeviceName;

import java.io.File;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import org.libsdl.app.SDLActivity;

import pl.cntrpl.voidsprite.util.Utils;

public class VSPActivity extends SDLActivity {

    public static String packageName = "";
    public static String deviceName = "";
    public static VSPActivity activitySingleton = null;

    public List<String> mainArgs = new ArrayList<String>();

    @Override
    protected String[] getArguments() {

        return mainArgs.toArray(new String[0]);
    }

    @Override
    protected void main() {

        packageName = getPackageName();
        activitySingleton = this;

        Intent intent = getIntent();
        if (Intent.ACTION_VIEW.equals(intent.getAction())) {
            Uri uri = intent.getData();
            if (uri != null) {
                mainArgs.add(uri.toString());
            }
        }

        String appdataPath = getExternalFilesDir(null).getAbsolutePath() + "/";
        passAppdataPathString(appdataPath);

        DeviceName.with(this).request((info, error) -> {
            deviceName = info.getName();
        });

        String systemInformation = "Android " + android.os.Build.VERSION.RELEASE + " (SDK " + android.os.Build.VERSION.SDK_INT + ")\n";
        systemInformation += String.format("CPU supported ABIs: %s\n  os.arch: %s\n", String.join(", ", Build.SUPPORTED_ABIS), System.getProperty("os.arch"));
        systemInformation += String.format("%s (%s %s | %s %s)\n", deviceName, Build.MANUFACTURER, Build.PRODUCT, Build.MODEL, Build.DEVICE);
        if (Build.VERSION.SDK_INT >= 31) {
            systemInformation += String.format("SOC: %s %s\n", Build.SOC_MANUFACTURER, Build.SOC_MODEL);
        }
        passSystemInformationString(systemInformation);

        super.main();
    }

    public static boolean hasFileAccessPermission() {
        return Build.VERSION.SDK_INT >= 30 ? Environment.isExternalStorageManager()
                : (ContextCompat.checkSelfPermission(activitySingleton, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PermissionChecker.PERMISSION_GRANTED);
    }

    public static void requestAllFilesPermission() {
        if (Build.VERSION.SDK_INT >= 30) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
            intent.setData(Uri.parse("package:" + packageName));
            activitySingleton.startActivity(intent);
        } else {
            ActivityCompat.requestPermissions(activitySingleton, new String[] { Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE }, 1);
        }
    }

    public static void openUrl(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setData(Uri.parse(url));
        activitySingleton.startActivity(intent);
    }

    public static void openFileLocation(String path) {
        //todo: fix this unfinished mess that doesn't work
        //  it can't open specific paths (keeps opening /sdcard/Downloads if you try)
        //  these are the only paths i can get it to open:
        //  content://com.android.externalstorage.documents/root/primary/ : opens internal memory root
        //  content://com.android.externalstorage.documents/root/<drive id>/ : opens external storage root 
        //  content://pl.cntrpl.voidsprite[.debug].provider/root : opens voidsprite provider root (appdata folder) (doesn't work on android 9)
        try {
            String extDataPath = activitySingleton.getExternalFilesDir(null).getAbsolutePath();

            if (new File(path).isFile()) {
                path = path.substring(0, path.lastIndexOf('/'));
            }

            if (path.startsWith(extDataPath)) {
                path = "content://"+ activitySingleton.getPackageName() +".provider/root/" + path.substring(extDataPath.length());
            }
            else {
                /*String contentRoot = "content://com.android.externalstorage.documents/root/";
                if (path.startsWith("/sdcard/")) {
                    path = contentRoot + "primary/" + path.substring("/sdcard/".length());
                } else if (path.startsWith("/storage/emulated/")) {
                    int trimFrom = path.indexOf('/', "/storage/emulated/".length()) + 1;
                    path = contentRoot + "primary/" + path.substring(trimFrom);
                } else if (path.startsWith("/storage/")) {
                    path = contentRoot + path.substring("/storage/".length());
                } else {
                    //idk man
                    path = contentRoot;
                }*/
                //todo: make this work
                path = "";
            }

            if (!path.isEmpty()) {
                ArrayList<Pair<String, String>> fileActivitiesToTry = new ArrayList<>();
                //ngl i got these package ids by decompiling com.marc.files
                //android 9 doesn't seem to have the com.google.android.documentsui package and needs the second one
                fileActivitiesToTry.add(new Pair<>("com.google.android.documentsui", "com.android.documentsui.files.FilesActivity"));
                fileActivitiesToTry.add(new Pair<>("com.android.documentsui", "com.android.documentsui.files.FilesActivity"));

                for (Pair<String,String> act : fileActivitiesToTry) {
                    try {
                        Uri uri = Uri.parse(path);
                        Intent i = new Intent(Intent.ACTION_VIEW, uri);
                        i.setComponent(new ComponentName(act.first, act.second));
                        activitySingleton.startActivity(i);
                        break;
                    } catch (Exception ignored) {}
                }
            }
        } catch (Exception e) {
            new Thread(() -> {
                Looper.prepare();
                Toast.makeText(activitySingleton, "Error:\n " + e.getMessage(), Toast.LENGTH_LONG).show();
            }).start();
        }
    }

    public static String fetchStringHTTP(String url) {
        return Utils.fetchStringHTTP(url);
    }
    public static byte[] fetchDataHTTP(String url) {
        return Utils.fetchDataHTTP(url);
    }

    public static native void passAppdataPathString(String appdataPath);
    public static native void passSystemInformationString(String systemInformation);
}
