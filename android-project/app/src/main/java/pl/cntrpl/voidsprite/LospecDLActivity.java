package pl.cntrpl.voidsprite;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Looper;
import android.widget.Toast;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.util.ArrayList;

import pl.cntrpl.voidsprite.util.Utils;

public class LospecDLActivity extends Activity {

    private File downloadsDir;

    class LospecDLTask extends Thread {
        private final Activity context;
        private final String fullUri;

        public LospecDLTask(Activity context, String fullUri) {
            this.context = context;
            this.fullUri = fullUri;
        }

        @Override
        public void run() {
            Looper.prepare();
            String targetName = fullUri.substring("lospec-palette://".length());
            String apiUrl = "https://lospec.com/palette-list/" + targetName + ".json";
            File outputFile = new File(downloadsDir, targetName + ".voidplt");

            String response = Utils.fetchStringHTTP(apiUrl);
            if (response != null) {
                try {
                    JSONObject jsonResponse = new JSONObject(response);
                    JSONArray colorsArray = jsonResponse.getJSONArray("colors");
                    ArrayList<Integer> colorList = new ArrayList<>();
                    for (int i = 0; i < colorsArray.length(); i++) {
                        String colorHex = colorsArray.getString(i);
                        int colorInt = Integer.parseUnsignedInt(colorHex, 16);
                        colorList.add(colorInt);
                    }
                    if (Utils.writeVOIDPLTv1(outputFile, colorList)) {
                        String message = "Palette downloaded: " + targetName;
                        Toast.makeText(context, message, Toast.LENGTH_LONG).show();
                    } else {
                        String errorMessage = "Failed to write palette file: " + targetName;
                        Toast.makeText(context, errorMessage, Toast.LENGTH_LONG).show();
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                    String errorMessage = "Download failed";
                    Toast.makeText(context, errorMessage, Toast.LENGTH_LONG).show();
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

        downloadsDir = new File(getExternalFilesDir(null) + "/palettes/");

        //setContentView(R.layout.activity_download);
        //createNotifChannel();

        Intent intent = getIntent();
        if (Intent.ACTION_VIEW.equals(intent.getAction())) {
            Uri uri = intent.getData();
            //Toast.makeText(this, "Download started: " + uri.getHost(), Toast.LENGTH_LONG).show();

            new LospecDLTask(this, uri.toString()).start();
        }

    }
}
