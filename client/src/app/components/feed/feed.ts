import { Component, effect, OnInit, inject } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule, MatSelectChange } from '@angular/material/select';
import { FeedService } from '@services/feed/feed';
import { TimeService } from '@services/time/time';
import { HeaderComponent } from "@components/header/header";


@Component({
    selector: 'app-feed',
    standalone: true,
    imports: [MatFormFieldModule, MatInputModule, MatSelectModule, MatIconModule, MatButtonModule, HeaderComponent],
    templateUrl: './feed.html',
    styleUrl: './feed.scss'
})
export class FeedPage implements OnInit {
    protected readonly feedService: FeedService = inject(FeedService);

    constructor() {
        effect(() => {
            const id = this.feedService.selectedListingId();

            if (id === null)
                return;

            if (!this.feedService.filteredListings().some((listing) => listing.id === id))
                this.feedService.selectedListingId.set(null);
        });
    }

    public ngOnInit(): void {
        this.feedService.fetchFeed();
    }

    public clearFilters(): void {
        this.feedService.clearFilters()
    }

    public closeDetail(): void {
        this.feedService.selectedListingId.set(null);
    }

    public expiresLabel(timestampSeconds: number): string | null {
    if (!timestampSeconds)
        return null;

        const diffMs = timestampSeconds * 1000 - Date.now();
        if (diffMs <= 0)
            return 'Closed';

        const diffDays = Math.ceil(diffMs / 86_400_000);
        if (diffDays === 1)
            return 'Closes tomorrow';
        if (diffDays <= 14)
            return `Closes in ${diffDays}d`;
        return null;
    }

    public formatCategory(category: string): string {
        return category.replace(/-/g, ' ');
    }

    public selectListing(id: number): void {
        this.feedService.selectedListingId.set(id);
    }

    public timeAgo(timestampSeconds: number): string {
        return TimeService.timeAgo(timestampSeconds);
    }

    public updateCategory(change: MatSelectChange): void {
        this.feedService.selectedCategory.set(change.value);
    }

    public updateLocation(change: MatSelectChange): void {
        this.feedService.selectedLocation.set(change.value);
    }

    public updateSearch(value: string): void {
        this.feedService.searchTerm.set(value);
    }
}