package pl.cntrpl.voidsprite;

import android.Manifest;
import android.app.Application;
import android.content.pm.ActivityInfo;
import android.net.Uri;
import android.os.Build;
import android.content.Intent;
import android.os.Environment;
import android.provider.Settings;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.content.PermissionChecker;

import com.jaredrummler.android.device.DeviceName;

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

    public static String fetchStringHTTP(String url) {
        return Utils.fetchStringHTTP(url);
    }
    public static byte[] fetchDataHTTP(String url) {
        return Utils.fetchDataHTTP(url);
    }

    public static native void passAppdataPathString(String appdataPath);
    public static native void passSystemInformationString(String systemInformation);
}
