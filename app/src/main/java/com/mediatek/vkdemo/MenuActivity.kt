package com.mediatek.vkdemo

import android.content.Intent
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.ArrayAdapter
import android.widget.ListView
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.Toolbar
import androidx.core.view.ViewCompat
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import com.google.android.material.dialog.MaterialAlertDialogBuilder

class MenuActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_menu)

        // 讓內容延伸到系統列（狀態列/導覽列）底下
        WindowCompat.setDecorFitsSystemWindows(window, false)

        // Toolbar
        val toolbar = findViewById<Toolbar>(R.id.toolbar).also { setSupportActionBar(it) }
        supportActionBar?.title = "VK Demo (v${BuildConfig.VERSION_NAME})"
        applyStatusBarInset(toolbar)

        // ListView
        val demoTitles = listOf("GLES Clear Sample")
        val demoIds = listOf("gles/clear")

        val listView = findViewById<ListView>(R.id.listView).apply {
            // 讓最後一個 item 不被 navigation bar 擋住
            clipToPadding = false
            applyBottomInset(this)

            adapter = ArrayAdapter(this@MenuActivity, android.R.layout.simple_list_item_1, demoTitles)
            setOnItemClickListener { _, _, position, _ ->
                startActivity(
                    Intent(this@MenuActivity, MainActivity::class.java)
                        .putExtra("demo_id", demoIds[position]) // 保證有值
                )
            }
        }
    }

    /** 將狀態列 inset 加到 Toolbar（以初始值為基準，避免重複累加） */
    private fun applyStatusBarInset(toolbar: Toolbar) {
        val baseHeight = toolbar.layoutParams.height
        val basePaddingTop = toolbar.paddingTop

        ViewCompat.setOnApplyWindowInsetsListener(toolbar) { v, insets ->
            val top = insets.getInsets(WindowInsetsCompat.Type.statusBars()).top
            v.setPadding(v.paddingLeft, basePaddingTop + top, v.paddingRight, v.paddingBottom)
            v.layoutParams = v.layoutParams.apply { height = baseHeight + top }
            insets
        }
        ViewCompat.requestApplyInsets(toolbar)
    }

    /** 將 navigation bar 高度加到底部 padding，避免內容被遮住 */
    private fun applyBottomInset(view: View) {
        val basePaddingBottom = view.paddingBottom
        ViewCompat.setOnApplyWindowInsetsListener(view) { v, insets ->
            val bottom = insets.getInsets(WindowInsetsCompat.Type.navigationBars()).bottom
            v.setPadding(v.paddingLeft, v.paddingTop, v.paddingRight, basePaddingBottom + bottom)
            insets
        }
        ViewCompat.requestApplyInsets(view)
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        return when (item.itemId) {
            R.id.action_about -> {
                showAboutDialog()
                true
            }
            else -> super.onOptionsItemSelected(item)
        }
    }

    private fun showAboutDialog() {
        val msg = getString(
            R.string.about_message,
            BuildConfig.VERSION_NAME,
            BuildConfig.VERSION_CODE,
            "TzuHsien Wang",
            BuildConfig.APPLICATION_ID
        )
        MaterialAlertDialogBuilder(this)
            .setTitle("About")
            .setMessage(msg)
            .setPositiveButton(android.R.string.ok, null)
            .show()
    }
}
