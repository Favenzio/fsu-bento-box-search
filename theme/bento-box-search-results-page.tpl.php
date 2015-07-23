<?php

/**
 * @file
 * Default theme implementation for search results page.
 */

?>

<p>Your search for <strong><?php print $search_input; ?></strong> 
returned the following results:</p>

<div id="search-results-container">

<?php foreach($search_sources as $search_source): ?>

<div id="<?php print $search_source->nid; ?>" class="bento-search-system-container">
  <h4>From <?php print $search_source->title; ?></h4>
  <div id="ajax-loader-<?php print $search_source->nid; ?>" class="ajax-loader">
      <img src="<?php print '/' . drupal_get_path('module', 'bento_box_search') . '/images/ajax-loader.gif' ?>">
  </div>
</div>

<?php endforeach; ?>

</div>