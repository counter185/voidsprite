package pl.cntrpl.voidsprite;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentSender;
import android.content.pm.PackageInstaller;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.os.Looper;
import android.provider.DocumentsProvider;
import android.util.Log;
import android.widget.Toast;

import androidx.core.content.FileProvider;

import pl.cntrpl.voidsprite.util.InstallerReceiver;
import pl.cntrpl.voidsprite.util.Utils;

public class UpdateVSPActivity extends Activity {

    class VSPDownloadAPKTask extends Thread {
        private final Activity context;

        public VSPDownloadAPKTask(Activity context) {
            this.context = context;
        }

        @Override
        public void run() {
            Looper.prepare();
            String url = "https://nightly.link/counter185/voidsprite/workflows/msbuild/main/voidsprite-build-android.zip";
            byte[] data = Utils.fetchDataHTTP(url);
            if (data == null) {
                Toast.makeText(context, "Failed to download update", Toast.LENGTH_LONG).show();
            }
            else {
                File tempFile = new File(context.getExternalCacheDir(), "vsp_update.zip");
                try {
                    Utils.writeDataToFile(tempFile, data);
                    ZipFile zipFile = new ZipFile(tempFile);
                    ZipEntry entry = zipFile.getEntry("voidsprite-release.apk");
                    InputStream stm = zipFile.getInputStream(entry);


                    try {
                        //File outFile = new File(context.getExternalFilesDir(null), "voidsprite-release.apk");
                        byte[] apkFile = Utils.readAllBytes(stm);
                        //Utils.writeDataToFile(outFile, Utils.readAllBytes(stm));

                        PackageInstaller installer = context.getApplicationContext().getPackageManager().getPackageInstaller();
                        int sessionID = installer.createSession(new PackageInstaller.SessionParams(PackageInstaller.SessionParams.MODE_FULL_INSTALL));
                        try (PackageInstaller.Session installerSession = installer.openSession(sessionID)) {
                            try (OutputStream output = installerSession.openWrite(context.getApplicationContext().getPackageName(), 0, -1)) {
                                output.write(apkFile);
                            }

                            Intent intent = new Intent(context.getApplicationContext(), InstallerReceiver.class);
                            intent.setAction("APP_INSTALL_ACTION");
                            PendingIntent pendingIntent = PendingIntent.getBroadcast(
                                context,
                                sessionID,
                                intent,
                            (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S ? PendingIntent.FLAG_MUTABLE : 0) | PendingIntent.FLAG_UPDATE_CURRENT
                            );

                            installerSession.commit(pendingIntent.getIntentSender());
                            Log.d("vsp-updater", "installerSession.commit");
                        }

                    } finally {
                        stm.close();
                        zipFile.close();
                        tempFile.delete();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    Toast.makeText(context, "Failed to process update:\n" + e.getMessage(), Toast.LENGTH_LONG).show();
                }
            }

            context.finish();
            Looper.loop();
            Looper.myLooper().quitSafely();
        }
    }

    @Override
    protected void onCreate(android.os.Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_update_vsp);

        new VSPDownloadAPKTask(this).start();
    }
}
