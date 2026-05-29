// Mount a persistent IDBFS filesystem at /saves before main() runs, so the
// game's <name>.sav files (written by ti_persist_var in port/platform/tifile.c)
// survive a page reload. The bundled game data under /data is a read-only MEMFS
// preload and is intentionally left alone.
//
// addRunDependency blocks the start of main() until the initial syncfs(true)
// has populated /saves from IndexedDB, so saved games are visible immediately.
Module["preRun"] = Module["preRun"] || [];
Module["preRun"].push(function () {
  try {
    FS.mkdir("/saves");
  } catch (e) {
    /* already exists */
  }
  FS.mount(IDBFS, {}, "/saves");
  addRunDependency("idbfs-load");
  FS.syncfs(true, function (err) {
    if (err) console.error("syncfs load:", err);
    removeRunDependency("idbfs-load");
  });

  // Safety: flush any pending saves to IndexedDB when the page is hidden/closed.
  window.addEventListener("pagehide", function () { try { FS.syncfs(false, function () {}); } catch (e) {} });
  document.addEventListener("visibilitychange", function () {
    if (document.visibilityState === "hidden") { try { FS.syncfs(false, function () {}); } catch (e) {} }
  });
});
