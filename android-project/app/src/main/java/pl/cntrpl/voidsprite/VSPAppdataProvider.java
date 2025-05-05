package pl.cntrpl.voidsprite;

import android.database.Cursor;
import android.database.MatrixCursor;
import android.os.CancellationSignal;
import android.os.ParcelFileDescriptor;
import android.provider.DocumentsContract;
import android.provider.DocumentsProvider;
import android.util.Log;

import androidx.annotation.Nullable;

import java.io.File;
import java.io.FileNotFoundException;

public class VSPAppdataProvider extends DocumentsProvider {

    //i'm so sorry dolphin emulator but i just have no idea what i'm doing here
    //https://github.com/dolphin-emu/dolphin/blob/master/Source/Android/app/src/main/java/org/dolphinemu/dolphinemu/features/DocumentProvider.kt

    private File root = null;
    private String rootDir = "";
    private String rootId = "root";
    private String[] DEFAULT_ROOT_PROJECTION = {
            DocumentsContract.Root.COLUMN_ROOT_ID,
            DocumentsContract.Root.COLUMN_FLAGS,
            DocumentsContract.Root.COLUMN_ICON,
            DocumentsContract.Root.COLUMN_TITLE,
            DocumentsContract.Root.COLUMN_DOCUMENT_ID
        };
    @Override
    public Cursor queryRoots(String[] projection) throws FileNotFoundException {
        MatrixCursor ret = new MatrixCursor(projection == null ? DEFAULT_ROOT_PROJECTION : projection);
        ret.newRow()
                .add(DocumentsContract.Root.COLUMN_ROOT_ID, rootId)
                .add(DocumentsContract.Root.COLUMN_TITLE, "voidsprite")
                .add(DocumentsContract.Root.COLUMN_ICON, R.mipmap.ic_launcher)
                .add(DocumentsContract.Root.COLUMN_FLAGS,
                        DocumentsContract.Root.FLAG_LOCAL_ONLY
                        | DocumentsContract.Root.FLAG_SUPPORTS_CREATE)
                .add(DocumentsContract.Root.COLUMN_DOCUMENT_ID, rootId);

        return ret;
    }

    @Override
    public Cursor queryDocument(String s, String[] strings) throws FileNotFoundException {
        MatrixCursor ret = new MatrixCursor(strings == null ? DEFAULT_ROOT_PROJECTION : strings);
        File file = documentIdToPath(s);
        if (file.isDirectory()) {
            ret.newRow()
                    .add(DocumentsContract.Document.COLUMN_DOCUMENT_ID, file.getName())
                    .add(DocumentsContract.Document.COLUMN_SIZE, file.length())
                    .add(DocumentsContract.Document.COLUMN_DISPLAY_NAME, file.getName())
                    .add(DocumentsContract.Document.COLUMN_MIME_TYPE, DocumentsContract.Document.MIME_TYPE_DIR)
                    .add(DocumentsContract.Document.COLUMN_FLAGS, DocumentsContract.Document.FLAG_DIR_SUPPORTS_CREATE);
        } else {
            ret.newRow()
                    .add(DocumentsContract.Document.COLUMN_DOCUMENT_ID, file.getName())
                    .add(DocumentsContract.Document.COLUMN_SIZE, file.length())
                    .add(DocumentsContract.Document.COLUMN_DISPLAY_NAME, file.getName())
                    .add(DocumentsContract.Document.COLUMN_MIME_TYPE, "application/octet-stream")
                    .add(DocumentsContract.Document.COLUMN_SIZE, file.length());
        }
        return ret;
    }

    @Override
    public Cursor queryChildDocuments(String parentDocumentId, String[] projection, String s1) throws FileNotFoundException {
        MatrixCursor ret = new MatrixCursor(projection == null ? DEFAULT_ROOT_PROJECTION : projection);
        Log.i("VSPAppdataProvider", "query: " + parentDocumentId);
        File folder = documentIdToPath(parentDocumentId);
        File[] files = folder.listFiles();
        if (files != null) {
            for (File file : files) {
                //this does not want to print anything
                Log.i("VSPAppdataProvider", "File: " + file.getAbsolutePath());
                if (file.isDirectory()) {
                    ret.newRow()
                            .add(DocumentsContract.Document.COLUMN_DOCUMENT_ID, file.getName())
                            .add(DocumentsContract.Document.COLUMN_DISPLAY_NAME, file.getName())
                            .add(DocumentsContract.Document.COLUMN_MIME_TYPE, DocumentsContract.Document.MIME_TYPE_DIR)
                            .add(DocumentsContract.Document.COLUMN_FLAGS, DocumentsContract.Document.FLAG_DIR_SUPPORTS_CREATE);
                } else {
                    ret.newRow()
                            .add(DocumentsContract.Document.COLUMN_DOCUMENT_ID, file.getName())
                            .add(DocumentsContract.Document.COLUMN_DISPLAY_NAME, file.getName())
                            .add(DocumentsContract.Document.COLUMN_MIME_TYPE, "application/octet-stream")
                            .add(DocumentsContract.Document.COLUMN_SIZE, file.length());
                }
            }
        }
        ret.setNotificationUri(getContext().getContentResolver(), DocumentsContract.buildChildDocumentsUri(
                "${context!!.packageName}.user", parentDocumentId));
        return ret;
    }

    @Override
    public ParcelFileDescriptor openDocument(String documentId, String mode, @Nullable CancellationSignal cancellationSignal) throws FileNotFoundException {
        return ParcelFileDescriptor.open(documentIdToPath(documentId), ParcelFileDescriptor.parseMode(mode));
    }

    @Override
    public boolean onCreate() {
        rootDir = getContext().getExternalFilesDir(null).getAbsolutePath() + "/";
        root = new File(rootDir);
        return true;
    }

    private File documentIdToPath(String documentId) throws FileNotFoundException {
        if (documentId.equals(rootId)) {
            return root;
        }
        int startIndex = rootId.length();
        File file = new File(root, documentId.substring(startIndex));
        if (!file.exists()) {
            //throw new FileNotFoundException(String.format("File %s does not exist.", documentId));
        }
        return file;
    }
}
