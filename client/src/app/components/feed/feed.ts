import { Component, effect, OnInit, inject } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule, MatSelectChange } from '@angular/material/select';
import { ThemeService } from '@services/theme/theme';
import { ThemeComponent } from '@components/theme/theme';
import { FeedService } from '@services/feed/feed';


@Component({
    selector: 'app-feed',
    standalone: true,
    imports: [MatFormFieldModule, MatInputModule, MatSelectModule, MatIconModule, MatButtonModule, ThemeComponent],
    templateUrl: './feed.html',
    styleUrl: './feed.scss',
    host: {
        '[attr.data-theme]': 'theme()',
    },
})
export class FeedPage implements OnInit {
    private readonly themeService: ThemeService = inject(ThemeService);
    protected readonly feedService: FeedService = inject(FeedService);

    constructor() {
        effect(this.themeService.saveTheme.bind(this.themeService));

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

    public get theme() {
        return this.themeService.theme;
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
        if (!timestampSeconds)
            return 'Recently posted';

        const diffMilliseconds = Date.now() - timestampSeconds * 1000;
        const diffMinutes = Math.floor(diffMilliseconds / 60_000);

        if (diffMinutes < 1)
            return 'Just now';
        if (diffMinutes < 60)
            return `${diffMinutes}m ago`;

        const diffHours = Math.floor(diffMinutes / 60);
        if (diffHours < 24)
            return `${diffHours}h ago`;

        const diffDays = Math.floor(diffHours / 24);
        if (diffDays < 30)
            return `${diffDays}d ago`;

        const diffMonths = Math.floor(diffDays / 30);
        if (diffMonths < 12)
            return `${diffMonths}mo ago`;

        return `${Math.floor(diffMonths / 12)}y ago`;
    }

    public toggleTheme(): void {
        this.themeService.toggle();
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