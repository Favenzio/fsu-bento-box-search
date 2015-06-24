<?php

/**
 * @file
 * Default theme implementation for search results page.
 */

?>

<p>Your search for <strong><?php print $search_input; ?></strong> 
returned the following results:</p>

<div id="search-results-container-left">

<?php for($i = 0; $i < $ss_half_round; $i++): ?>

<div id="<?php print $search_sources[$i]->nid; ?>" class="bento-search-system-container">
  <h4>From the <?php print $search_sources[$i]->title; ?></h4>
  <div id="ajax-loader-<?php print $search_sources[$i]->nid; ?>" class="ajax-loader">
      <img src="<?php print '/' . drupal_get_path('module', 'bento_box_search') . '/images/ajax-loader.gif' ?>">
  </div>
</div>

<?php endfor; ?>

</div>

<div id="search-results-container-right">

<?php for($i = $ss_half_round; $i < $ss_count; $i++): ?>

<div id="<?php print $search_sources[$i]->nid; ?>" class="bento-search-system-container">
  <h4>From the <?php print $search_sources[$i]->title; ?></h4>
  <div id="ajax-loader-<?php print $search_sources[$i]->nid; ?>" class="ajax-loader">
      <img src="<?php print '/' . drupal_get_path('module', 'bento_box_search') . '/images/ajax-loader.gif' ?>">
  </div>  
</div>

<?php endfor; ?>

</div>