<?php

/**
 * @file
 * Default theme implementation for search results page.
 */

?>

<p>Your search for <strong><?php print $search_input; ?></strong> 
returned the following results:</p>

<?php foreach($search_sources as $search_source): ?>
<h4>From <?php print $search_source->title; ?></h4>
<div id="<?php print $search_source->nid; ?>"></div>
<?php endforeach; ?>